#include "Renderer.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "../utils/globals.hpp"
#include <algorithm>



void Renderer::Init()
{
    GX = (editor.sceneView.SCENEVIEW_WIDTH + 23.0f) / 24;
    GY = (editor.sceneView.SCENEVIEW_HEIGHT + 23.0f) / 24;
    GZ = 24;
    CLUSTER_COUNT = GX * GY * GZ;

    glCreateBuffers(SDL_arraysize(SSBOs), SSBOs);

    glNamedBufferStorage(lightSSBO, sizeof(GPUPointLight) * MAX_LIGHTS, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferData(clusterSSBO, sizeof(glm::vec4) * 2 * CLUSTER_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glNamedBufferData(countSSBO, sizeof(uint32_t) * CLUSTER_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glNamedBufferData(indexSSBO, sizeof(uint32_t) * MAX_PER_CLUSTER * CLUSTER_COUNT, nullptr, GL_DYNAMIC_DRAW);
    clusterWidth = editor.sceneView.SCENEVIEW_WIDTH;
    clusterHeight = editor.sceneView.SCENEVIEW_HEIGHT;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, clusterSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, countSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, indexSSBO);
    gpuDirLights.reserve(MAX_DIRLIGHTS);

    // set fragment shader properties for cluster rendering
    engine.litShader.use();
    engine.litShader.setFloat("zNear", editor.editorCamera._near);
    engine.litShader.setFloat("zFar", editor.editorCamera._far);
    engine.litShader.setVec3u("gridSize", glm::uvec3(GX, GY, GZ));
    engine.litShader.setVec2("uTileSize", glm::vec2(editor.sceneView.SCENEVIEW_WIDTH, editor.sceneView.SCENEVIEW_HEIGHT) / glm::vec2(GX, GY));

    fallbackmap.Initialize(whitepixel, 1, 1, 4);
    normalfallback.Initialize(normal, 1, 1, 4);
}

void Renderer::OpenGLRenderer(Scene* _scene) {
    // renders the scene in a opengl context
    const bool sceneChanged = previousScene != _scene;
    scene = _scene;
    previousScene = _scene;

    // change cluster dimentions on size change 
    bool clustersResized = false;
    if (clusterWidth != editor.sceneView.SCENEVIEW_WIDTH || clusterHeight != editor.sceneView.SCENEVIEW_HEIGHT) {
        clusterWidth = editor.sceneView.SCENEVIEW_WIDTH;
        clusterHeight = editor.sceneView.SCENEVIEW_HEIGHT;
        GX = std::max(1u, static_cast<uint32_t>((clusterWidth + 23) / 24));
        GY = std::max(1u, static_cast<uint32_t>((clusterHeight + 23) / 24));
        CLUSTER_COUNT = GX * GY * GZ;
        glNamedBufferData(clusterSSBO, sizeof(glm::vec4) * 2 * CLUSTER_COUNT, nullptr, GL_DYNAMIC_DRAW);
        glNamedBufferData(countSSBO, sizeof(uint32_t) * CLUSTER_COUNT, nullptr, GL_DYNAMIC_DRAW);
        glNamedBufferData(indexSSBO, sizeof(uint32_t) * MAX_PER_CLUSTER * CLUSTER_COUNT, nullptr, GL_DYNAMIC_DRAW);
        engine.litShader.use();
        engine.litShader.setVec3u("gridSize", glm::uvec3(GX, GY, GZ));
        engine.litShader.setVec2("uTileSize", glm::vec2(clusterWidth, clusterHeight) / glm::vec2(GX, GY));
        clustersResized = true;
    }

    // build AABB -> load lights -> calculate clusters (AABB + lights) -> render scene
    const bool projectionChanged = firstFrame || clustersResized || editor.editorCamera.projDirty;
    if (projectionChanged) {
        // projection related propertes in shader
        engine.litShader.setFloat("zNear", editor.editorCamera._near);
        engine.litShader.setFloat("zFar", editor.editorCamera._far);
        
        editor.editorCamera.getProjMat();
        calculateAABBClusters();
    }

    const bool lightsDirty = scene->rendererDirty || sceneChanged || firstFrame;
    if (lightsDirty) {
        loadLights();
    }

    if (editor.editorCamera.viewDirty || lightsDirty || projectionChanged || firstFrame) {
        calculateClusterLights();
    }

    renderScene();

    if (firstFrame) firstFrame = false;
}

void Renderer::loadLights() {
    // point lights -> pointlight SSBO
    static std::vector<GPUPointLight> gpuPointLights;
    gpuPointLights.clear();

    // Components can be added or removed after a scene is loaded. Rebuild the
    // light list from the scene's current objects so the renderer never uses
    // stale component pointers and newly added lights become active.
    scene->lights.clear();
    for (const auto& gameObject : scene->gameObjects) {
        if (LightComponent* light = gameObject->GetComponent<LightComponent>()) {
            scene->lights.push_back(light);
        }
    }

    gpuDirLights.clear();
    gpuDirLights.reserve(MAX_DIRLIGHTS);

    uint8_t nDirLight = 0;

    for (auto& light : scene->lights) {
        if (light->type == LightType::DIRECTIONAL) {
            if (nDirLight >= MAX_DIRLIGHTS) {
                continue;
            }
            gpuDirLights.push_back({ light->parent->transform->rotation * glm::vec3(0.0f, 0.0f, -1.0f), light->color });
            nDirLight++;
        }
        else {
            if (gpuPointLights.size() >= MAX_LIGHTS) continue;
            gpuPointLights.push_back({ glm::vec4(light->parent->transform->position, light->range), glm::vec4(light->color, light->intensity) });
        };
    }

    // point lights -> SSBO: upload only the lights you actually have
    engine.litShader.use();
    LIGHT_COUNT = gpuPointLights.size();
    engine.litShader.setInt("nPointLight", LIGHT_COUNT);
    glNamedBufferSubData(lightSSBO, 0, LIGHT_COUNT * sizeof(GPUPointLight), gpuPointLights.data());

    // directional light -> UBOs
    engine.litShader.setInt("nDirLights", nDirLight);

    for (int i = 0; i < nDirLight; i++) {
        std::string base = "uDirLights[" + std::to_string(i) + "]";

        engine.litShader.setVec3((base + ".color").c_str(), gpuDirLights[i].color);
        engine.litShader.setVec3((base + ".direction").c_str(), gpuDirLights[i].direction);
    }

    gpuDirLights.clear();
}

void Renderer::calculateAABBClusters() {
    // Calculate the forward clusters
    static ComputeShader aabbBuilderProg(SHADER_DIR "lit/aabb_builder.comp");

    glm::mat4& proj = editor.editorCamera.projectionMat;
    float tanHalfFov = 1.0f / proj[1][1];
    float aspect = proj[1][1] / proj[0][0];

    aabbBuilderProg.use();
    aabbBuilderProg.setFloat("tanHalfFov", tanHalfFov);
    aabbBuilderProg.setFloat("aspect", aspect);
    aabbBuilderProg.setFloat("zNear", editor.editorCamera._near);
    aabbBuilderProg.setFloat("zFar", editor.editorCamera._far);
    aabbBuilderProg.setVec3u("gridSize", glm::uvec3(GX, GY, GZ));
    aabbBuilderProg.setUInt("clusterSize", CLUSTER_COUNT);

    constexpr int LOCAL_SIZE = 64;
    GLuint count = GX * GY * GZ;
    glDispatchCompute((count + LOCAL_SIZE - 1) / LOCAL_SIZE, 1, 1); // extra dispatch for undivisible clusters

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Renderer::calculateClusterLights() {
    // Calculate the light clusters
    static ComputeShader lightClusterProg(SHADER_DIR "lit/cluster_cull.comp");
    glClearNamedBufferData(countSSBO, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    lightClusterProg.use();
    lightClusterProg.setUInt("nPointLight", LIGHT_COUNT);
    lightClusterProg.setUInt("clusterCount", CLUSTER_COUNT);
    lightClusterProg.setMat4("uView", editor.editorCamera.getViewMat());

    glDispatchCompute((CLUSTER_COUNT + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

constexpr uint64_t Renderer::makeKey(uint32_t shader, uint32_t material, uint32_t mesh, uint32_t sub) {
    // Field	Bits	Max Optimal distinct values
    // shader	8	256
    // material	16	65, 536
    // mesh	    16	65,536
    // submesh	24	16,777,216

    return (uint64_t(shader & 0xFF) << 56) |
        (uint64_t(material & 0xFFFF) << 40) |
        (uint64_t(mesh & 0xFFFF) << 24) |
        uint64_t(sub & 0xFFFFFF);
}

// batch material. select material, apply transform, render mesh
void Renderer::renderScene() {
    static std::vector<BatchEntry> batchEntries;

    if (scene->rendererDirty) {
        batchEntries.clear();
        for (auto& go : scene->gameObjects) {
            if (!go->HasComponent<MeshComponent>()) continue;
            Mesh* runtimeMeshAsset = engine.runtimeAssetsRegistry.getAsset<Mesh>(go->GetComponent<MeshComponent>()->meshUUID);
            if (runtimeMeshAsset == nullptr) continue;
            std::vector<Submesh>& submeshes = reinterpret_cast<Mesh*>(runtimeMeshAsset)->submeshes;

            for (int i = 0; i < submeshes.size(); i++) {
                MaterialAsset* materialAsset = engine.assetsRegistry.getAsset<MaterialAsset>(submeshes[i].material);
                if (materialAsset == nullptr) continue;

                // Process each submesh of a gameobject
                BatchEntry entry;
                entry.key = makeKey(static_cast<uint32_t>(materialAsset->shaderType), engine.assetsRegistry.m_assetsMeta.find(submeshes[i].material), engine.assetsRegistry.m_assetsMeta.find(go->GetComponent<MeshComponent>()->meshUUID.low), i);

                entry.m_shaderType = materialAsset->shaderType;
                entry.m_material = materialAsset;
                entry.m_mesh = runtimeMeshAsset;
                entry.m_submesh = &submeshes[i];
                entry.m_transform = go->transform.get();
                batchEntries.push_back(entry);
            }
        }

        std::sort(batchEntries.begin(), batchEntries.end(), [](const BatchEntry& a, const BatchEntry& b) {
            return a.key < b.key;
        });
        scene->rendererDirty = false;
    }

    ShaderType lastShaderType = static_cast<ShaderType>(-1);
    MaterialAsset* lastMaterial = nullptr;
    glm::mat4& view = editor.editorCamera.getViewMat();
    glm::mat4& proj = editor.editorCamera.getProjMat();

    for (auto& batchEntry : batchEntries) {
        // select shader
        if (batchEntry.m_shaderType != lastShaderType) {
            lastShaderType = batchEntry.m_shaderType;
            if (lastShaderType == ShaderType::LIT) {
                engine.litShader.use();
                engine.litShader.setMat4("uView", view);
                engine.litShader.setMat4("uProjection", proj);
                engine.litShader.setVec3("uCamPos", editor.editorCamera.position);
            }
            else {
                engine.unlitShader.use();
                engine.unlitShader.setMat4("uView", view);
                engine.unlitShader.setMat4("uProjection", proj);
            }
        }

        // set material
        if (batchEntry.m_material != lastMaterial) {
            lastMaterial = batchEntry.m_material;

            MaterialAsset* matAsset = engine.assetsRegistry.getAsset<MaterialAsset>(batchEntry.m_submesh->material);

            if (lastShaderType == ShaderType::LIT) {
                engine.litShader.setVec3("uMaterial.diffuseColor", lastMaterial->diffuseColor);
                engine.litShader.setVec3("uMaterial.specularColor", lastMaterial->specularColor);
                engine.litShader.setFloat("uMaterial.shininess", lastMaterial->shininess);
                Texture* diffuseTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->diffuseMapUUID);
                Texture* specularTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->specularMapUUID);
                Texture* normalTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->normalMapUUID);

                if (diffuseTexture == nullptr) diffuseTexture = &fallbackmap;
                if (specularTexture == nullptr) specularTexture = &fallbackmap;
                if (normalTexture == nullptr) normalTexture = &normalfallback;

                diffuseTexture->bind(0);
                specularTexture->bind(1);
                normalTexture->bind(2);

                engine.litShader.setInt("uMaterial.uDiffuseMap", 0);
                engine.litShader.setInt("uMaterial.uSpecularMap", 1);
                engine.litShader.setInt("uMaterial.uNormalMap", 2);
            }
            else {
                engine.unlitShader.setVec3("uMaterial.diffuseColor", lastMaterial->diffuseColor);
                Texture* diffuseTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->diffuseMapUUID);
                if (diffuseTexture == nullptr) diffuseTexture = &fallbackmap;

                diffuseTexture->bind(0);
                engine.unlitShader.setInt("uMaterial.diffuseMap", 0);
            }
        }

        // set transform
        engine.litShader.setMat4("uModel", batchEntry.m_transform->getModel());
        if (lastShaderType == ShaderType::LIT) engine.litShader.setMat3("uNormalMatrix", glm::mat3(batchEntry.m_transform->getNormalMat()));

        // render mesh
        batchEntry.m_mesh->bind();
        glDrawElements(GL_TRIANGLES, batchEntry.m_submesh->indicesSize, GL_UNSIGNED_INT, (void*)(batchEntry.m_submesh->indicesOffset * sizeof(uint32_t)));
        lastMaterial = batchEntry.m_material;
    }

}
