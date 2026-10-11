#pragma once
#include "glad/gl.h"
#include "string"
#include "AssetRuntime.hpp"
#include "../meta/TextureAsset.hpp"

struct TexDets {
    unsigned int wrapS = GL_CLAMP_TO_EDGE;
    unsigned int wrapT = GL_CLAMP_TO_EDGE;
    unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
    unsigned int magFilter = GL_NEAREST;
};

class Texture : public RuntimeAsset {
public:
    unsigned int TexId = GL_NONE;
    std::string texturePath = "";
    Texture() = default;

	AssetType getType() const override {
		return AssetType::TEXTURE;
	}

	virtual TextureType getTextureType() const = 0;

    // bind texture to a texture unit
	void bind(unsigned int unit = 0) const;

    // move symantics
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};

class Texture2D : public Texture {
public:
    Texture2D() = default;
    Texture2D(const char* path, TexDets texDets);
    Texture2D(const unsigned char* data, int width, int height, int channels, TexDets texDetails = TexDets());
    void Initialize(const fs::path& path, TexDets texDetails = TexDets());
    void Initialize(const unsigned char* data, int width, int height, int channels, TexDets texDetails = TexDets());
    void applyParams(TexDets dets);

    bool Serialize(const AssetMeta& entry) override;

    TextureType getTextureType() const override {
        return TextureType::TEXTURE_2D;
    }
private:
    void setTexParam(unsigned int Param, unsigned int Value) const;
    void UploadToGL(const unsigned char* data, int width, int height, int channels, TexDets texDets, bool generateMips);
};

class Cubemap : public Texture {
public:
    Cubemap() = default;
    void Initialize(const fs::path& path, TexDets texDetails = TexDets());
    void applyParams(TexDets dets);

    TextureType getTextureType() const override {
        return TextureType::CUBEMAP;
    }
    
    bool Serialize(const AssetMeta& entry) override;

private:
    void setTexParam(unsigned int Param, unsigned int Value) const;
};