#pragma once
#include "GLAD/gl.h"
#include "Shader.hpp"

struct SceneView {
    int SCENEVIEW_WIDTH;
    int SCENEVIEW_HEIGHT;

    unsigned int textureObj, framebuffObj, depthbuffObj;
};

extern SceneView sceneView;

extern Shader litShader;
extern Shader unlitShader;