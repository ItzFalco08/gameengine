#include "AssetMeta.hpp"
#include "glm/glm.hpp"

struct MaterialAsset : public AssetMeta {
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;

	UUID ambientMapUUID{ 0 };
	UUID diffuseMapUUID{ 0 };
	UUID specularMapUUID{ 0 };
	UUID normalMapUUID{ 0 };

	AssetType getType() const override {
		return AssetType::MATERIAL;
	}
	bool Serialize(const fs::path& assetFile) override;
};