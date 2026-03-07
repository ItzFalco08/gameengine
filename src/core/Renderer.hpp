#include "Scene.hpp"
#include "Camera.hpp"
#include "../utils/Shader.hpp"
extern Shader* litShader;
extern Shader* unlitShader;
extern Camera editorCam;

class Renderer {
public:
    Renderer() = default;

    void OpenGLRenderer(Scene* _scene) {
        scene = _scene;
        // renders the scene in a opengl context

        loadLights(); 
    }
private:
    Scene* scene;

    void loadLights() {
        litShader->use();

        int n = 0;
        for(const auto& light : scene->lights) {
            if(n==49)break;
            
            LightType type = light->getLightType();

            switch (type)
            {
            case LightType::POINT: {
                PointLight* pointLight = static_cast<PointLight*>(light->lightProps.get());
                std::string base = "uPointLights[" + std::to_string(n) + "]";
                litShader->setVec3((base + ".color").c_str(), pointLight->lightColor);
                litShader->setVec3((base + ".position").c_str(), light->parent->transform->position);
                litShader->setFloat((base + ".intensity").c_str(), pointLight->intensity);
                litShader->setFloat((base + ".range").c_str(), pointLight->range);
                break;
            }
            
            case LightType::DIRECTIONAL: {
                DirLight* dirLight = static_cast<DirLight*>(light->lightProps.get());
                std::string base = "uDirLights[" + std::to_string(n) + "]";
                glm::vec3 direction = glm::rotate(light->parent->transform->rotation, glm::vec3(0.0f, 0.0f, -1.0f));
                litShader->setVec3((base + ".color").c_str(), dirLight->lightColor);
                litShader->setVec3((base + ".direction").c_str(), direction);
                break;
            }
            default:
                break;
            }

            n+=1;
        }
    };
};
