#pragma once
#include "GLAD/gl.h"
#include "string"

struct TexDets {
    unsigned int wrapS = GL_CLAMP_TO_EDGE;
    unsigned int wrapT = GL_CLAMP_TO_EDGE;
    unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
    unsigned int magFilter = GL_NEAREST;
};

class Texture {
public:
    unsigned int TexId = GL_NONE;
    std::string texturePath = "";

    Texture() = default;
    Texture(const char* path, TexDets texDetails = TexDets());
    // move symantics
    Texture(Texture&& other) noexcept;
    void operator=(Texture&& other) noexcept;
    ~Texture();

    void setTexParam(unsigned int Param, unsigned int Value) const;
    bool Initialize(const char* path, TexDets texDetails = TexDets());

private:
    void applyParams(TexDets dets);
};