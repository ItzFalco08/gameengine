#include "AssetMeta.hpp"

struct MeshAsset : public AssetMeta {
	fs::path m_sourceFile;
	uint32_t m_vertexCount;
	uint32_t m_indexCount;
	uint32_t m_submeshCount;

	AssetType getType() const override {
		return AssetType::MESH;
	}

	bool Serialize(const fs::path& assetFile) override;
};