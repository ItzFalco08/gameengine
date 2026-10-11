#pragma once

struct SceneView {
    int SCENEVIEW_WIDTH = 100;
    int SCENEVIEW_HEIGHT = 100;

    unsigned int textureObj, framebuffObj, depthbuffObj;

    float getAspect() const {
        if (SCENEVIEW_HEIGHT == 0) return 1.0f;
        return static_cast<float>(SCENEVIEW_WIDTH) / static_cast<float>(SCENEVIEW_HEIGHT);
    }
};
