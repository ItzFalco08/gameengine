#pragma once
#include "AssetRuntime.hpp"
#include <glad/gl.h>

struct Mesh : public RuntimeAsset {
	GLuint VAO, VBO, EBO{ GL_NONE };

	AssetType getType() const override {
		return AssetType::MESH;
	}

	bool Serialize(const AssetMeta& entry) override {
		return true;
	}
};