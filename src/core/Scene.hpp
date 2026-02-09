#pragma once
#include "GameObject.hpp"
#include "json/json.hpp"
#include <fstream>

class Scene { // used single instance of Scene for the GameEngine Editor
public:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<GameObject*> roots; 
    std::string sceneName;
    std::string sceneFilePath;
    bool dirty = false; // Used for Editor (keeps track for changes)
    
    // S C E N E -> J S O N
    void Serialize() { // Save Scene ( Only Used for Editor to change scene.json )
        if (!dirty) { LOG::Error("No Changes are Made yet"); return; }
        if (sceneFilePath.empty()) { LOG::Error("no scene file provided"); };
    
        nlohmann::json sceneJson;

        nlohmann::json gameObjectsArray = nlohmann::json::array();
        sceneJson["sceneName"] = sceneName;

        for (const auto& gameObject : gameObjects) {
            nlohmann::json gameObjectJson;
            gameObject->Serialize(gameObjectJson);
            gameObjectsArray.push_back(std::move(gameObjectJson));
        }

        sceneJson["gameObjects"] = std::move(gameObjectsArray);

        // upload to file
        std::ofstream file(sceneFilePath);
        if (!file.is_open()) {
            LOG::Error("Failed to open scene file: ", sceneFilePath);
            return;
        }

        file << sceneJson.dump(4);
        file.close();

        dirty = false;
    }

    // J S O N -> S C E N E
    void Deserialize(std::string scenePath) { // Load Scene from json (used from runtime game script)
        gameObjects.clear();
        sceneFilePath = scenePath;
        std::ifstream file(sceneFilePath);

        if(!file.is_open()) {
            LOG::Error("Failed to Load scene: ", sceneFilePath);
            return;
        }

        nlohmann::json sceneJson;
        file >> sceneJson;

        sceneName = sceneJson.at("sceneName");

        // extract gameObjects
        auto& gameObjectsArray = sceneJson.at("gameObjects");

        for(auto& gameObjectJson : gameObjectsArray) {
            std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>(this, gameObjectJson.at("name"));
            
            gameObject->Deserialize(gameObjectJson);

            gameObjects.push_back(std::move(gameObject));
        }
    }

    GameObject* AddGameObject(const std::string& goName) {
        auto newGo = std::make_unique<GameObject>(this, goName);
        GameObject* ptr = newGo.get();                          
        
        gameObjects.push_back(std::move(newGo));                 // Move ownership to vector
        
        dirty = true;
        return ptr;
    }; 

    void RemoveGameObject() {
        dirty = true;
        // future implementation
    };

    //  make scene dirty on changing the gameObject internally
    void MakeDirty() {
        dirty = true;
    }

    const char* GetNameString() {
        static std::string nameStr;
        nameStr = sceneName + (dirty ? "*" : "");
        return nameStr.c_str();
    };

};
