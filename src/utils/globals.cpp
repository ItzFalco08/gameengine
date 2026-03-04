#include "globals.hpp"

SceneView sceneView = {800, 600, 0, 0, 0}; // Initialize with default dimensions

Shader litShader;
Shader unlitShader;

Scene* editorScene = new Scene();
GameObject* selectedGameObject;

AssetsManager assetsManager;
ResourceManager resourceManager;

namespace panels {
	ScenePanel scenePanel;
	AssetsBrowser assetsBrowserPanel;
	HeriarchyPanel heriarchyPanel;
	InspectorPanel inspectorPanel;
}