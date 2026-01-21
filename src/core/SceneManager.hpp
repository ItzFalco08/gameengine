#include "vector"
#include "Scene.hpp"
#include "sstream"
#include "fstream"

class SceneManager { // switching between scenes made easy. (USEFUL DURING RUNTIME)
    std::unordered_map<std::string, std::unique_ptr<Scene>> loadedScenes;
    Scene* activeScene; // passed to renderer

    void setActiveScene(std::string scenePath) {
        activeScene = loadedScenes[scenePath].get();
    }

    void loadScene(const char* path) {
        auto scene = std::make_unique<Scene>();
        
        scene->Serialize(path);
        loadedScenes[path] = std::move(scene);
    }

    void SaveScene(const char* path) {
        loadedScenes[path]->Deserialize(path);
    }

    ~SceneManager() {
        delete activeScene;
        // loaded scenes deleted automatically
    }
};
