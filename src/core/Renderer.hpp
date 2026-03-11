#include "Scene.hpp"
#include "Camera.hpp"
#include "components/Mesh.hpp"
#include "components/Material.hpp"
#include "../utils/globals.hpp"
#include "../utils/Shader.hpp"

extern Shader litShader;
extern Shader unlitShader;

class Renderer {
public:
    Renderer() = default;

    void OpenGLRenderer(Scene* _scene) {
        scene = _scene;
        // renders the scene in a opengl context

        loadLights();
        renderScene();
    }
private:
    Scene* scene;
    GameObject* curGo;
    Material* mat;

    // loads the to the lit shader;
    void loadLights() {
        litShader.use();

        int nPoint = 0;
        int nDir = 0;
        for(const auto& light : scene->lights) {
            LightType type = light->getLightType();

            switch (type)
            {
            case LightType::POINT: {
                if(nPoint >= 49) break;
                PointLight* pointLight = static_cast<PointLight*>(light->lightProps.get());
                std::string base = "uPointLights[" + std::to_string(nPoint) + "]";
                litShader.setVec3((base + ".color").c_str(), pointLight->lightColor);
                litShader.setVec3((base + ".position").c_str(), light->parent->transform->position);
                litShader.setFloat((base + ".intensity").c_str(), pointLight->intensity);
                litShader.setFloat((base + ".range").c_str(), pointLight->range);
                nPoint++;
                break;
            }
            
            case LightType::DIRECTIONAL: {
                if(nDir >= 6) break;
                DirLight* dirLight = static_cast<DirLight*>(light->lightProps.get());
                std::string base = "uDirLights[" + std::to_string(nDir) + "]";
                glm::vec3 direction = glm::rotate(light->parent->transform->rotation, glm::vec3(0.0f, 0.0f, -1.0f));
                litShader.setVec3((base + ".color").c_str(), dirLight->lightColor);
                litShader.setVec3((base + ".direction").c_str(), direction);
                nDir++;
                break;
            }
            default:
                break;
            }
        }
    };

    void renderScene() {
        for (const auto& gameObject : scene->gameObjects) {
            curGo = gameObject.get();
            if (!isGoRenderable()) continue;
            
            setShaderProps(); // set uMaterial in corresponding shader
            renderMesh(); // render the mesh
        }
    };

    bool isGoRenderable() {
        return curGo->hasComponent<Material>() && curGo->hasComponent<Mesh>();
    }

    void setShaderProps() {
        mat = curGo->GetComponent<Material>();
        MaterialType curMatType = mat->getMaterialType();

        switch (curMatType) {
        case MaterialType::LIT:
            litShader.use();
            setLitShaderProps();
            break;
        case MaterialType::UNLIT:
            unlitShader.use();
            setUnlitShaderProps();
            break;
        }
    }

    void setLitShaderProps() {

        litShader.setMat4("model", curGo->transform->getModel());
        litShader.setMat3("normalMatrix", glm::mat3(curGo->transform->getNormalMat()));
        litShader.setMat4("view", editorCamera->getViewMat());
        litShader.setMat4("projection", editorCamera->getProjMat());
        litShader.setVec3("uCamPos", editorCamera->position);


        // set material
        LitMaterial* litMat = static_cast<LitMaterial*>(mat->matprops.get());
        litShader.setVec3("uMaterial.color", litMat->ambientColor);
        litShader.setFloat("uMaterial.ambientStrength", litMat->ambientStrength);
        litShader.setFloat("uMaterial.diffuseStrength", litMat->diffuseStrength);
        litShader.setFloat("uMaterial.specularStrength", litMat->specularStrength);
        litShader.setFloat("uMaterial.shininess", litMat->shininess);
        
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, resourceManager.LoadAndGetTexture(litMat->texturePath)->TexId);

    }

    void setUnlitShaderProps() {                        
        unlitShader.use();
        unlitShader.setMat4("model", curGo->transform->getModel());
        unlitShader.setMat4("normalMatrix", curGo->transform->getNormalMat());
        unlitShader.setMat4("view", editorCamera->getViewMat());
        unlitShader.setMat4("projection", editorCamera->getProjMat());

        // set material
        UnlitMaterial* unlitMat = static_cast<UnlitMaterial*>(mat->matprops.get());
        unlitShader.setVec3("uMaterial.color", unlitMat->ambientColor);

        glActiveTexture(GL_TEXTURE0);
    }
  
    void renderMesh() {
        if (auto* meshPtr = curGo->GetComponent<Mesh>()) {
            // renders the mesh
            glBindVertexArray(meshPtr->VAO);
            glDrawArrays(GL_TRIANGLES, 0, meshPtr->vertexCount);
            glBindVertexArray(0);
        }
    }
};