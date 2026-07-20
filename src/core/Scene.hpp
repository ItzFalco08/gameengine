#pragma once
#include <filesystem>
#include "GameObject.hpp"
#include "./components/Light.hpp"
#include "json/json.hpp"
#include <fstream>

class Scene {
public:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<GameObject*> roots;
    std::vector<Light*> lights;
    std::filesystem::path sceneFilePath;
    bool dirty = false;

    // S C E N E -> J S O N
    void Serialize();

    // J S O N -> S C E N E
    void Deserialize(std::string scenePath);
    
    // api
    GameObject* AddGameObject(const std::string& goName, GameObject* parent);
    void RemoveGameObject(GameObject* gameObject);
    void MakeDirty();
    void SaveScene();

    const char* GetNameString();
private:
    void recursiveJsonGen(nlohmann::json& gameObjectsArray, std::vector<GameObject*>& nodes);
    void recursion(std::vector<GameObject*>& nodes, nlohmann::json& goArr, GameObject* parent);
};
