#include "globals.hpp"
#include "../core/Renderer.hpp"
#include "../gui/ScenePanel.hpp"
#include "../gui/AssetsBrowserPanel.hpp"
#include "../core/SceneView.hpp"

SceneView sceneView = { 800, 600, 0, 0, 0 }; // Initialize with default dimensions

Shader litShader;
Shader unlitShader;

Scene* editorScene = new Scene();
Camera* editorCamera = new Camera();
GameObject* selectedGameObject;

AssetsManager assetsManager;
ResourceManager resourceManager;
Renderer renderer;

namespace panels {
	ScenePanel scenePanel;
	AssetsBrowser assetsBrowserPanel;
	HeriarchyPanel heriarchyPanel;
	InspectorPanel inspectorPanel;
}
