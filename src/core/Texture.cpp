#include "Texture.hpp"
#include "../utils/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

Texture::Texture(const char* path, TexDets texDets) {
    int width = 0, height = 0, nrChannels = 0;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    unsigned int format = GL_RGB;
    bool wantsMips = (
        texDets.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        texDets.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        texDets.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        texDets.minFilter == GL_NEAREST_MIPMAP_LINEAR
    );

    if (data) {
        switch (nrChannels) {
            case 1: format = GL_RED;  break;
            case 2: format = GL_RG;   break;
            case 3: format = GL_RGB;  break;
            case 4: format = GL_RGBA; break;
            default: format = GL_RGB; break;
        }
    } else {
        LOG::Error("Failed to load texture: ", path);
        // Fallback 1x1 magenta pixel
        static const unsigned char fallback[4] = {255, 0, 255, 255};
        width = 1; height = 1; nrChannels = 4; format = GL_RGBA;

        glGenTextures(1, &TexId);
        glBindTexture(GL_TEXTURE_2D, TexId);
        applyParams(texDets);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, fallback);
        if (wantsMips) glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    glGenTextures(1, &TexId);
    glBindTexture(GL_TEXTURE_2D, TexId);
    applyParams(texDets);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (wantsMips) glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::applyParams(TexDets dets) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dets.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dets.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, dets.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, dets.magFilter);   
}