#pragma once
#include "AssetMeta.hpp"
#include "glm/glm.hpp"

enum class ShaderType {
	LIT,
	UNLIT
};

struct MaterialAsset : public AssetMeta {
	ShaderType shaderType = ShaderType::LIT;
	glm::vec3 diffuseColor; // baseColor
	glm::vec3 specularColor; // specular highlight Color
	float shininess;

	UUID diffuseMapUUID{ 0 };
	UUID specularMapUUID{ 0 };
	UUID normalMapUUID{ 0 };

	AssetType getType() const override {
		return AssetType::MATERIAL;
	}
	bool Serialize(const fs::path& assetFile) override;
};