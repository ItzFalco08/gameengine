#pragma once
#include "AssetRuntime.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp> 

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Submesh {
	char name[512] = "";
	uint32_t indicesOffset = 0;
	uint32_t indicesSize = 0;
	UUID material = 0;
};

struct Mesh : public RuntimeAsset {
	GLuint VAO, VBO, EBO{ GL_NONE };
	std::vector<Submesh> submeshes;

	AssetType getType() const override {
		return AssetType::MESH;
	}

	bool Serialize(const AssetMeta& entry) override;
};