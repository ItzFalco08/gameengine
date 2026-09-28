#include "AssetMeta.hpp"

struct MeshAsset : public AssetMeta {
	fs::path sourceFile;

	AssetType getType() const override {
		return AssetType::MESH;
	}

	bool Serialize(const fs::path& assetFile) override;
};