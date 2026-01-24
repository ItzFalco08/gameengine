#pragma once
#include "GameObject.hpp"
#include "json/json.hpp"
#include <fstream>

struct Scene { // used single instance of Scene for the GameEngine Editor
    std::string sceneName;
    std::string sceneFilePath;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
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
            std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>(this);
            
            gameObject->Deserialize(gameObjectJson);

            gameObjects.push_back(std::move(gameObject));
        }
    }

    void AddGameObject() {
        gameObjects.push_back(std::make_unique<GameObject>(this));
        dirty = true;
    }; 
    void RemoveGameObject() {
        dirty = true;
        // future implementation
    };

    //  make scene dirty on changing the gameObject internally
    void MakeDirty() {
        dirty = true;
    }
};
