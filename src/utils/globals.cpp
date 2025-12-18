#include "globals.hpp"
#include "../gui/ScenePanel.hpp"
#include "../gui/AssetsBrowserPanel.hpp"
#include "../core/AssetsManager.hpp"

SceneView sceneView = {800, 600, 0, 0, 0}; // Initialize with default dimensions

Shader litShader;
Shader unlitShader;
AssetsManager assetsManager;

namespace panels {
	ScenePanel scenePanel;
	AssetsBrowser assetsBrowserPanel;
}