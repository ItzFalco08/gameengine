#pragma once
#include "GLAD/gl.h"
#include "Shader.hpp"
#include "../core/AssetsManager.hpp"
#include "../core/Scene.hpp"

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
extern Scene* editorScene;

namespace panels {
    extern ScenePanel scenePanel;
    extern AssetsBrowser assetsBrowserPanel;
};
