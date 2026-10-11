#include "Renderer.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "../utils/globals.hpp"



void Renderer::Init()
{
    GX = (editor.sceneView.SCENEVIEW_WIDTH + 23.0f) / 24;
    GY = (editor.sceneView.SCENEVIEW_HEIGHT + 23.0f) / 24;
    GZ = 24;
    CLUSTER_COUNT = GX * GY * GZ;

    glCreateBuffers(SDL_arraysize(SSBOs), SSBOs);

    glNamedBufferStorage(lightSSBO, sizeof(GPUPointLight) * MAX_LIGHTS, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(clusterSSBO, sizeof(glm::vec4) * 2 * CLUSTER_COUNT, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(countSSBO, sizeof(uint32_t) * CLUSTER_COUNT, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(indexSSBO, sizeof(uint32_t) * MAX_PER_CLUSTER * CLUSTER_COUNT, nullptr, GL_DYNAMIC_STORAGE_BIT);
    gpuDirLights.reserve(MAX_DIRLIGHTS);

    // set fragment shader properties for cluster rendering
    engine.litShader.setFloat("zNear", editor.editorCamera._near);
    engine.litShader.setFloat("zFar", editor.editorCamera._far);
    engine.litShader.setVec3("gridSize", glm::vec3(GX, GY, GZ));
    engine.litShader.setVec2("uTileSize", glm::vec2(editor.sceneView.SCENEVIEW_WIDTH, editor.sceneView.SCENEVIEW_HEIGHT) / glm::vec2(GX, GY));

    fallbackmap.Initialize(whitepixel, 1, 1, 4);
    normalfallback.Initialize(normal, 1, 1, 4);
}

void Renderer::OpenGLRenderer(Scene* _scene) {
    // renders the scene in a opengl context
    scene = _scene;

    // build AABB -> load lights -> calculate clusters (AABB + lights) -> render scene
    if (firstFrame || editor.editorCamera.projDirty || editor.sceneView.getAspect() != prevAspect) {
        calculateAABBClusters();
    }

    if (scene->dirty || firstFrame) {
        loadLights();
        prevAspect = editor.sceneView.getAspect();
    }

    if (editor.editorCamera.viewDirty || firstFrame) {
        calculateClusterLights();
    }

    renderScene();

    if (firstFrame) firstFrame = false;
}

inline void Renderer::loadLights() {
    // point lights -> pointlight SSBO
    static std::vector<GPUPointLight> gpuPointLights;

    gpuDirLights.reserve(MAX_LIGHTS);

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
            gpuPointLights.push_back({ glm::vec4(light->parent->transform->position, light->range), glm::vec4(light->color, light->intensity) });
        };
    }

    // point lights -> SSBO: upload only the lights you actually have
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

inline void Renderer::calculateAABBClusters() {
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
    aabbBuilderProg.setUInt("clusterCount", CLUSTER_COUNT);

    constexpr int LOCAL_SIZE = 64;
    GLuint count = GX * GY * GZ;
    glDispatchCompute((count + LOCAL_SIZE - 1) / LOCAL_SIZE, 1, 1); // extra dispatch for undivisible clusters

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

inline void Renderer::calculateClusterLights() {
    // Calculate the light clusters
    static ComputeShader lightClusterProg(SHADER_DIR "lit/cluster_cull.comp");
    lightClusterProg.use();
    lightClusterProg.setUInt("nPointLight", LIGHT_COUNT);
    glDispatchCompute((CLUSTER_COUNT + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

inline constexpr uint64_t Renderer::makeKey(uint32_t shader, uint32_t material, uint32_t mesh, uint32_t sub) {
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
inline void Renderer::renderScene() {
    static std::vector<BatchEntry> batchEntries;

    for (auto& go : scene->gameObjects) {
        if (!go->hasComponent<MeshComponent>()) continue;
        Mesh* runtimeMeshAsset = engine.runtimeAssetsRegistry.getAsset<Mesh>(go->GetComponent<MeshComponent>()->meshUUID);
        if (runtimeMeshAsset == nullptr) continue;
        std::vector<Submesh>& submeshes = reinterpret_cast<Mesh*>(runtimeMeshAsset)->submeshes;

        for (int i = 0; i < submeshes.size(); i++) {
            MaterialAsset* materialAsset = engine.assetsRegistry.getAsset<MaterialAsset>(submeshes[i].material);

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

    static ShaderType lastShaderType = ShaderType::LIT;
    static MaterialAsset* lastMaterial = nullptr;
    static Submesh* lastSubmesh = nullptr;
    static bool firstBatch = true;

    for (auto& batchEntry : batchEntries) {
        // select shader
        if (batchEntry.m_shaderType != lastShaderType || firstBatch) {
            lastShaderType = batchEntry.m_shaderType;
            if (lastShaderType == ShaderType::LIT) {
                engine.litShader.use();
            }
            else {
                engine.unlitShader.use();
            }
            if (firstBatch) firstBatch = false;
        }

        // set material
        if (batchEntry.m_material != lastMaterial || firstBatch) {
            lastMaterial = batchEntry.m_material;

            MaterialAsset* matAsset = engine.assetsRegistry.getAsset<MaterialAsset>(batchEntry.m_submesh->material);

            if (lastShaderType == ShaderType::LIT) {
                engine.litShader.setVec3("uMaterial[diffuseColor]", lastMaterial->diffuseColor);
                engine.litShader.setVec3("uMaterial[specularColor]", lastMaterial->specularColor);
                engine.litShader.setFloat("uMaterial[shininess]", lastMaterial->shininess);
                Texture* diffuseTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->diffuseMapUUID);
                Texture* specularTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->specularMapUUID);
                Texture* normalTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->normalMapUUID);

                if (diffuseTexture == nullptr) diffuseTexture = &fallbackmap;
                if (specularTexture == nullptr) specularTexture = &fallbackmap;
                if (normalTexture == nullptr) normalTexture = &normalfallback;

                diffuseTexture->bind(0);
                engine.litShader.setInt("uMaterial[diffuseMap]", 0);
                specularTexture->bind(1);
                engine.litShader.setInt("uMaterial[specularMap]", 1);
                normalTexture->bind(2);
                engine.litShader.setInt("uMaterial[normalMap]", 2);
            }
            else {
                engine.unlitShader.setVec3("uMaterial[diffuseColor]", lastMaterial->diffuseColor);
                Texture* diffuseTexture = engine.runtimeAssetsRegistry.getAsset<Texture>(matAsset->diffuseMapUUID);
                if (diffuseTexture == nullptr) diffuseTexture = &fallbackmap;

                diffuseTexture->bind(0);
                engine.unlitShader.setInt("uMaterial[diffuseMap]", 0);
            }

            if (firstBatch) firstBatch = false;
        }

        // set transform
        glm::mat4& model = batchEntry.m_transform->getModel();
        glm::mat4& view = editor.editorCamera.getViewMat();
        glm::mat4& proj = editor.editorCamera.getProjMat();
        glm::mat4& normal = batchEntry.m_transform->getNormalMat();

        if (batchEntry.m_material != lastMaterial || firstBatch) {
            if (lastShaderType == ShaderType::LIT) {
                engine.litShader.setMat4("uModel", model);
                engine.litShader.setMat4("uView", view);
                engine.litShader.setMat4("uProjection", proj);
                engine.litShader.setMat4("uNormalMatrix", normal);
            }
            else {
                engine.unlitShader.setMat4("uModel", model);
                engine.unlitShader.setMat4("uView", view);
                engine.unlitShader.setMat4("uProjection", proj);
            }
        }

        // render mesh
        batchEntry.m_mesh->bind();
        glDrawElements(GL_TRIANGLES, batchEntry.m_submesh->indicesSize, GL_UNSIGNED_INT, (void*)(batchEntry.m_submesh->indicesOffset * sizeof(uint32_t)));
    }

    batchEntries.clear();
}
