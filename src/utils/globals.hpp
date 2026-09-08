#pragma once
#include "glad/gl.h"
#include "Shader.hpp"
#include "AssetsManager.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/SceneManager.hpp"
#include "../core/Camera.hpp"
#include "InputManager.hpp"
#include "../gui/HeriarchyPanel.hpp"
#include "../gui/InspectorPanel.hpp"
#include "../core/SceneView.hpp"

class ScenePanel;
class AssetsBrowser;
class Renderer;
struct GLFWwindow;

extern Shader litShader;
extern Shader unlitShader;
extern AssetsManager assetsManager;
extern ResourceManager resourceManager;

extern SceneManager sceneManager;
extern SceneView sceneView;
extern Camera editorCamera;
extern GameObject* selectedGameObject; 
extern Renderer renderer;
extern GLFWwindow* gMainWindow;

namespace panels {
    extern ScenePanel scenePanel;
    extern AssetsBrowser assetsBrowserPanel;
	extern HeriarchyPanel heriarchyPanel;
    extern InspectorPanel inspectorPanel;
};