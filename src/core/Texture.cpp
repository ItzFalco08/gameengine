#include "Texture.hpp"
#include "../utils/Logger.hpp"
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

void Texture2D::Initialize(const char* path, TexDets texDets) {
    if (texturePath == path) { LOG::Warning("Texture already bound with path: ", path); return; };
    texturePath = std::string(path);
    
    // load texture
    int width = 0, height = 0, nrChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    
    // fill fallback data
    bool usingFallback = false;
    unsigned int format;
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

// Cubemap
Cubemap::Cubemap(const std::vector<std::string>& paths, TexDets texDetails) {
    Initialize(paths, texDetails);
};

void Cubemap::Initialize(const std::vector<std::string>& paths, TexDets texDetails) {
    glGenTextures(1, &TexId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TexId);
    applyParams(texDetails);

    for (int i = 0; i < paths.size(); ++i) {
        int width = 0, height = 0, nrChannels = 0;
        uint8_t* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);

        bool using_fallback = false;
        if (!data) {
            LOG::Warning("failed to load texture from path: ", paths[i], ", fallbacking to 1x1 magenta");
            width = 1; height = 1; nrChannels = 4;
            static const uint8_t fallback[4] = { 255, 0, 255, 255 };
            data = (uint8_t*) &fallback;
            using_fallback = true;
        };
        
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );
        
        if(!using_fallback) stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
};

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