#pragma once
#include "GLAD/gl.h"
#include "Shader.hpp"
#include "../core/AssetsManager.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/Scene.hpp"
#include "InputManager.hpp"
#include "../gui/ScenePanel.hpp"
#include "../gui/AssetsBrowserPanel.hpp"
#include "../gui/HeriarchyPanel.hpp"
#include "../gui/InspectorPanel.hpp"

// Forward declarations to avoid circular includes
class ScenePanel;
class AssetsBrowser;

struct SceneView {
    int SCENEVIEW_WIDTH;
    int SCENEVIEW_HEIGHT;

    unsigned int textureObj, framebuffObj, depthbuffObj;
};

extern SceneView sceneView;

extern Shader litShader;
extern Shader unlitShader;
extern AssetsManager assetsManager;
extern InputManager inputManager;
extern ResourceManager resourceManager;

extern Scene* editorScene;
extern GameObject* selectedGameObject; 

namespace panels {
    extern ScenePanel scenePanel;
    extern AssetsBrowser assetsBrowserPanel;
	extern HeriarchyPanel heriarchyPanel;
    extern InspectorPanel inspectorPanel;
};
