#include "Texture.hpp"
#include "../../../utils/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

// Base Class
Texture::Texture(Texture&& other) noexcept {
    TexId = other.TexId;
    texturePath = other.texturePath;
    other.texturePath = "";
    other.TexId = GL_NONE;
};

Texture& Texture::operator=(Texture&& other) noexcept {
    if (TexId != GL_NONE) glDeleteTextures(1, &TexId);
    texturePath = other.texturePath;
    TexId = other.TexId;
    other.TexId = GL_NONE;
    other.texturePath = "";
    return *this;
}

Texture::~Texture() {
    if (TexId != GL_NONE) {
        glDeleteTextures(1, &TexId);
    }
}

// Texture2D
Texture2D::Texture2D(const char* path, TexDets texDets) {
    Initialize(path, texDets);
};

void Texture2D::Initialize(const fs::path& path, TexDets texDets) {
    if (texturePath == path) { LOG::Warning("Texture already bound with path: ", path); return; };
    texturePath = path.string();
    
    // load texture
    int width = 0, height = 0, nrChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 0);
    
    // fill fallback data
    bool usingFallback = false;
    if(!data) {
        width = 1; height = 1; nrChannels = 4;
        static const unsigned char fallback[4] = {255, 0, 255, 255};
        data = (unsigned char*)&fallback;
        usingFallback = true;
    }

    // GL texture info
    bool wantsMips = (
        texDets.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        texDets.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        texDets.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        texDets.minFilter == GL_NEAREST_MIPMAP_NEAREST
    );

    unsigned int format;
    switch (nrChannels) {
        case 1: format = GL_RED;  break;
        case 2: format = GL_RG;   break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default: format = GL_RGB; break;
    }

    // generate texture
    if (TexId == GL_NONE) glGenTextures(1, &TexId);
    
    glBindTexture(GL_TEXTURE_2D, TexId);
    applyParams(texDets);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (wantsMips && !usingFallback) glGenerateMipmap(GL_TEXTURE_2D);
    if(!usingFallback) stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::setTexParam(unsigned int Param, unsigned int Value) const {
    glBindTexture(GL_TEXTURE_2D, TexId);
    glTexParameteri(GL_TEXTURE_2D, Param, Value);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::applyParams(TexDets dets) {

    bool isTex = TexId != GL_NONE;
    if (isTex) glBindTexture(GL_TEXTURE_2D, TexId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dets.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dets.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, dets.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, dets.magFilter);

}

bool Texture2D::Serialize(const AssetMeta& entry) {
    TextureAsset& texAsset = (TextureAsset&)entry;
    texturePath = texAsset.m_texturePath.string();
    Initialize(texturePath, TexDets{
        texAsset.m_wrapU,
        texAsset.m_wrapV,
        texAsset.m_minFilter,
        texAsset.m_magFilter
        });
    return true;
}

// Cubemap
void Cubemap::Initialize(const fs::path& path, TexDets texDets) {
    if (texturePath == path) { LOG::Warning("Texture already bound with path: ", path); return; };
    if (path.extension() != ".hdr") {
        LOG::Error("Tried to load a non-hdr file as cubmap. CurrentBound: ", texturePath, " Applieds ", path);
        return;
    }
    texturePath = path.string();

    // load texture
    int width = 0, height = 0, nrChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(path.string().c_str(), &width, &height, &nrChannels, 0);

    // fill fallback data
    bool usingFallback = false;
    if (!data) {
        width = 1; height = 1; nrChannels = 4;
        static const float fallback[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
        data = (float*)&fallback;
        usingFallback = true;
    }

    // GL texture info
    bool wantsMips = (
        texDets.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        texDets.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        texDets.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        texDets.minFilter == GL_NEAREST_MIPMAP_NEAREST
    );

    unsigned int format;
    switch (nrChannels) {
        case 1: format = GL_RED;  break;
        case 2: format = GL_RG;   break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default: format = GL_RGB; break;
    }

    // generate texture
    if (TexId == GL_NONE) glGenTextures(1, &TexId);

    glBindTexture(GL_TEXTURE_2D, TexId);
    applyParams(texDets);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (wantsMips && !usingFallback) glGenerateMipmap(GL_TEXTURE_2D);
    if (!usingFallback) stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Cubemap::Serialize(const AssetMeta& entry) {
    TextureAsset& texAsset = (TextureAsset&)entry;
    texturePath = texAsset.m_texturePath.string();
    Initialize(texturePath, TexDets{
        texAsset.m_wrapU,
        texAsset.m_wrapV,
        texAsset.m_minFilter,
        texAsset.m_magFilter
    });
    return true;
}

void Cubemap::setTexParam(unsigned int Param, unsigned int Value) const {
    glBindTexture(GL_TEXTURE_2D, TexId);
    glTexParameteri(GL_TEXTURE_2D, Param, Value);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Cubemap::applyParams(TexDets dets) {

    bool isTex = TexId != GL_NONE;
    if (isTex) glBindTexture(GL_TEXTURE_2D, TexId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dets.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dets.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, dets.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, dets.magFilter);

}