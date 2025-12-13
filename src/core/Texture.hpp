#pragma once
#include "GLAD/gl.h"

struct TexDets {
    unsigned int wrapS = GL_CLAMP_TO_EDGE;
    unsigned int wrapT = GL_CLAMP_TO_EDGE;
    unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
    unsigned int magFilter = GL_NEAREST;
};

class Texture {
public:
    unsigned int TexId;

    Texture(const char* path, TexDets texDetails);

private:
    void applyParams(TexDets dets);
};