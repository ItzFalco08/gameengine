#pragma once
#include "glad/gl.h"
#include "string"
#include <vector>

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

    // move symantics
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    virtual ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};

class Texture2D : public Texture {
public:
    Texture2D() = default;
    Texture2D(const char* path, TexDets texDets);
    void Initialize(const char* path, TexDets texDetails = TexDets());
    void applyParams(TexDets dets);

private:
    void setTexParam(unsigned int Param, unsigned int Value) const;
};

class Cubemap : public Texture {
public:
    Cubemap() = default;
    Cubemap(const std::vector<std::string>& paths, TexDets texDetails = TexDets());
    void Initialize(const std::vector<std::string>& paths, TexDets texDetails = TexDets());
    void applyParams(TexDets dets);

private:
    void setTexParam(unsigned int Param, unsigned int Value) const;
};