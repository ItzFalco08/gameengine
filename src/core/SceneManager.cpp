#include "SceneManager.hpp"

void SceneManager::SetScene(const fs::path& path) {
    LOG::Info("Switching Scene");
    if (!fs::exists(path) || path.extension() != ".scene") {
        LOG::Error("Invalid scene path: ", path.string());
        return;
    }

    activeScene = std::make_unique<Scene>();
    activeScene->Deserialize(path.string());
}