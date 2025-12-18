#pragma once
#include "GLAD/gl.h"
#include "Shader.hpp"
#include "../core/AssetsManager.hpp"

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

namespace panels {
    extern ScenePanel scenePanel;
    extern AssetsBrowser assetsBrowserPanel;
};