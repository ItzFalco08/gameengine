#include "vector"
#include "Scene.hpp"
#include "Serializer.hpp"
#include "sstream"
#include "fstream"

class SceneManager {
    std::unordered_map<std::string, Scene> loadedScenes;
    Scene* activeScene;

    void setActiveScene(std::string sceneName) {
        activeScene = &loadedScenes[sceneName];
    }

    void loadScene(const char* path) {
        Scene scene;
        
        std::ifstream sceneJsonFile(path);
        nlohmann::json sceneJson;
        sceneJsonFile >> sceneJson;

        scene.sceneName = sceneJson["sceneName"];
        
        for (nlohmann::json gameObject : sceneJson["gameObjects"]) {
            
        }
    }

    void SaveScene() {
        nlohmann::json sceneJson;
        SerializerJSON scene(sceneJson); // js obj with write
        scene.Write("sceneName", activeScene->sceneName);

        for(auto& GameObject : activeScene->gameObjects) {
            
        }
    }
};