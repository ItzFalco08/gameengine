#pragma once
#include "GLAD/gl.h"
#include "Shader.hpp"
#include "../core/AssetsManager.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/SceneManager.hpp"
#include "../core/Camera.hpp"
#include "InputManager.hpp"
#include "../gui/HeriarchyPanel.hpp"
#include "../gui/InspectorPanel.hpp"

// Forward declarations to avoid circular includes
class ScenePanel;
class AssetsBrowser;
class Renderer;


extern Shader litShader;
extern Shader unlitShader;
extern AssetsManager assetsManager;
extern InputManager inputManager;
extern ResourceManager resourceManager;

extern SceneManager sceneManager;
extern Camera* editorCamera;
extern GameObject* selectedGameObject; 
extern std::string rootDir;
extern Renderer renderer;

namespace panels {
    extern ScenePanel scenePanel;
    extern AssetsBrowser assetsBrowserPanel;
	extern HeriarchyPanel heriarchyPanel;
    extern InspectorPanel inspectorPanel;
};

