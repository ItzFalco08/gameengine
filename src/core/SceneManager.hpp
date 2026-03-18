#pragma once
#include "Scene.hpp"
#include <filesystem>
#include "../utils/Logger.hpp"

namespace fs = std::filesystem;

class SceneManager {
public:
    std::unique_ptr<Scene> activeScene;

    SceneManager() : activeScene(std::make_unique<Scene>()) {}

    // api
    void SetScene(const fs::path& path);
};
