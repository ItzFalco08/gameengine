#include "Texture.hpp"
#include "../../../utils/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#include "../../../utils/Shader.hpp"
#ifndef SHADER_DIR
#define SHADER_DIR ""
#endif

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
GLuint hdriparser_fb = 0;

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

    // upload HDRI sampler texture to GPU
    GLuint hdriSamplerTex = GL_NONE;
    glGenTextures(1, &hdriSamplerTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdriSamplerTex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format == GL_RGB ? GL_RGB16F : GL_RGBA16F,
        width,
        height,
        0,
        format,
        GL_FLOAT,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texDets.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texDets.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texDets.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texDets.wrapT);

    if (wantsMips)
        glGenerateMipmap(GL_TEXTURE_2D);

	// create cubemap and render into it.
    const int cube_side = std::max(width / 4, 1);

    // target cubemap
    glGenTextures(1, &TexId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TexId);
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
            cube_side, cube_side, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, wantsMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // save state
    GLint prevFbo = 0, prevViewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
    glGetIntegerv(GL_VIEWPORT, prevViewport);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    GLenum drawBuffers[6];
    for (int i = 0; i < 6; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TexId, 0);
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(6, drawBuffers);

	static Shader hdriparser(SHADER_DIR "shaders/hdri_parser.vert", SHADER_DIR "shaders/hdri_parser.frag");

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        glViewport(0, 0, cube_side, cube_side);
        glDisable(GL_BLEND); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);

        hdriparser.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdriSamplerTex);
        hdriparser.setInt("hdriTexture", 0);

        float sq_vertices[] = { -1,-1,  1,-1,  -1,1,  1,1 };  // Z order

        GLuint vao = GL_NONE, vbo = GL_NONE;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sq_vertices), sq_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    else {
        LOG::Error("HDRI framebuffer is incomplete");
    }

    // restore + cleanup
    glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &hdriSamplerTex);
    if (!usingFallback) stbi_image_free(data);

    if (wantsMips) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, TexId);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
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