#include "Scene.hpp"
#include "SceneManager.hpp"
extern SceneManager sceneManager;

void Scene::recursiveJsonGen(nlohmann::json& gameObjectsArray, std::vector<GameObject*>& nodes) {
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
}

void Scene::Serialize() {
    if (!dirty) { LOG::Error("No Changes are Made yet"); return; }
    if (sceneFilePath.empty()) { LOG::Error("no scene file provided"); };

    nlohmann::json sceneJson;

    nlohmann::json gameObjectsArray = nlohmann::json::array();

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

void Scene::recursion(std::vector<GameObject*>& nodes, nlohmann::json& goArr, GameObject* parent) {
    if(goArr.empty()) return;

    for (auto& go : goArr ) {
        std::unique_ptr<GameObject> gameObjInstance = std::make_unique<GameObject>(this, go["name"]);
        gameObjInstance->Deserialize(go);
        gameObjInstance->parent = parent;

        nodes.push_back(gameObjInstance.get());
        recursion(gameObjInstance->childs, go["childs"], gameObjInstance.get());
        gameObjects.push_back(std::move(gameObjInstance));
    }
}

void Scene::Deserialize(std::string scenePath) {
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

    auto& gameObjectsArray = sceneJson.at("gameObjects");

    recursion(roots, gameObjectsArray, nullptr);
}

GameObject* Scene::AddGameObject(const std::string& goName, GameObject* parent) {
    auto newGo = std::make_unique<GameObject>(this, goName);
    GameObject* ptr = newGo.get();

    // set parent
    newGo->parent = parent;
    // adds to memory
    gameObjects.push_back(std::move(newGo));
    // adds to tree
    parent ? parent->childs.push_back(ptr) : roots.push_back(ptr);
    
    dirty = true;
    return ptr;
}

void Scene::RemoveGameObject(GameObject* gameObject) {
    if(gameObject == nullptr) return;

    // Make sure the pointer belongs to this scene before dereferencing it.
    const bool existsInScene = std::any_of(
        gameObjects.begin(),
        gameObjects.end(),
        [gameObject](const std::unique_ptr<GameObject>& go) {
            return go.get() == gameObject;
        }
    );

    if (!existsInScene) {
        LOG::Warning("RemoveGameObject called with non-owned or stale pointer");
        return;
    }

    // Detach from parent/root tree first.
    std::vector<GameObject*>& rootsRef = gameObject->parent ? gameObject->parent->childs : roots;
    auto it = std::find(rootsRef.begin(), rootsRef.end(), gameObject);
    if (it != rootsRef.end()) {
        rootsRef.erase(it);
    }

    // Recursively remove children while this object is still alive.
    auto childsCopy = gameObject->childs;
    for (GameObject* child : childsCopy) {
        RemoveGameObject(child);
    }

    // Erase from owned memory last to avoid use-after-free.
    auto objectIt = std::find_if(
        gameObjects.begin(),
        gameObjects.end(),
        [gameObject](const std::unique_ptr<GameObject>& go) {
            return go.get() == gameObject;
        }
    );
    if (objectIt != gameObjects.end()) {
        gameObjects.erase(objectIt);
    }

    dirty = true;
}

void Scene::MakeDirty() {
    dirty = true;
}

void Scene::SaveScene() {
    LOG::Info("Saving Scene: ", sceneFilePath.stem());
    Serialize();
    dirty = false;
}

const char* Scene::GetNameString() {
    static std::string nameStr;
    nameStr = sceneFilePath.stem().string() + (dirty ? "*" : "");
    return nameStr.c_str();
}
