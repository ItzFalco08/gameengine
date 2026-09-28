#include "AssetMeta.hpp"
#include "glad/gl.h"

enum TextureType {
	TEXTURE_2D,
	CUBEMAP
};

struct TextureAsset : public AssetMeta {
	fs::path m_texturePath;
	TextureType m_textureType;
	GLuint m_wrapU;
	GLuint m_wrapV;
	GLuint m_minFilter;
	GLuint m_magFilter;

	AssetType getType() const override {
		return AssetType::TEXTURE;
	}

	bool Serialize(const fs::path& assetFile) override;
};