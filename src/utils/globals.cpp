#include "globals.hpp"
#include <GLFW/glfw3.h>
#include "../core/Renderer.hpp"
#include "../gui/ScenePanel.hpp"
#include "../gui/AssetsBrowserPanel.hpp"
#include "../core/SceneView.hpp"

SceneView sceneView = { 800, 600, 0, 0, 0 }; // Initialize with default dimensions

Shader litShader;
Shader unlitShader;

SceneManager sceneManager;
Camera editorCamera;
GameObject* selectedGameObject;

AssetsManager assetsManager;
ResourceManager resourceManager;
Renderer renderer;

GLFWwindow* gMainWindow = nullptr;

std::string rootDir;

namespace panels {
	ScenePanel scenePanel;
	AssetsBrowser assetsBrowserPanel;
	HeriarchyPanel heriarchyPanel;
	InspectorPanel inspectorPanel;
}
