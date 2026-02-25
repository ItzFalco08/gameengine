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

    void recursiveJsonGen(nlohmann::json& gameObjectsArray, std::vector<GameObject*>& nodes) {
        if(nodes.empty()) return;

        for (const auto& node: nodes) {
            nlohmann::json gameObjJson;

            node->Serialize(gameObjJson);

            // child recursion
            nlohmann::json childJsonArr = nlohmann::json::array();
            recursiveJsonGen(childJsonArr, node->childs);
            gameObjJson["childs"] = std::move(childJsonArr);

            gameObjectsArray.push_back(std::move(gameObjJson));
        }
    };

    // S C E N E -> J S O N
    void Serialize() { // Save Scene ( Only Used for Editor to change scene.json )
        if (!dirty) { LOG::Error("No Changes are Made yet"); return; }
        if (sceneFilePath.empty()) { LOG::Error("no scene file provided"); };
    
        nlohmann::json sceneJson;

        nlohmann::json gameObjectsArray = nlohmann::json::array();
        sceneJson["sceneName"] = sceneName;

        // recursively serialize gameObjects 
        recursiveJsonGen(gameObjectsArray, roots);
        
        sceneJson["gameObjects"] = std::move(gameObjectsArray);

        // upload to file
        std::ofstream file(sceneFilePath);
        if (!file.is_open()) {
            LOG::Error("Failed to open scene file: ", sceneFilePath);
            return;
        }

        file << sceneJson.dump(2);
        file.close();

        dirty = false;
    }

    void recursion(std::vector<GameObject*>& nodes, nlohmann::json& goArr) {
        if(goArr.empty()) return;

        for (auto& go : goArr ) {
            // extract go;
            std::unique_ptr<GameObject> gameObjInstance = std::make_unique<GameObject>(this, go["name"]);
            gameObjInstance->Deserialize(go);

            nodes.push_back(gameObjInstance.get());
            recursion(gameObjInstance->childs, go["childs"]);
            gameObjects.push_back(std::move(gameObjInstance));
        }
    };

    // J S O N -> S C E N E
    void Deserialize(std::string scenePath) { // Load Scene from json (used from runtime game script)
        if (scenePath == sceneFilePath) {
            LOG::Warning("Scene Already Loaded! (Tried to load a already loaded active scene): ", scenePath);
            return;
        }
        gameObjects.clear();
        roots.clear();

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

        // recursively add all gameObjects from json into scene
        recursion(roots, gameObjectsArray);
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

    void SaveScene() {
        Serialize(); // serialize into where its serialized from
        dirty = false;
    }

    const char* GetNameString() {
        static std::string nameStr;
        nameStr = sceneName + (dirty ? "*" : "");
        return nameStr.c_str();
    };

};
