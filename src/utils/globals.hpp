#pragma once
#include "GLAD/gl.h"

struct SceneView {
    int SCENEVIEW_WIDTH;
    int SCENEVIEW_HEIGHT;

    unsigned int textureObj, framebuffObj, depthbuffObj;
};

extern SceneView sceneView;