#pragma once
#include "../../../utils/Utils.hpp"
#include <filesystem>
namespace fs = std::filesystem;

enum AssetType {
	MESH,
	MATERIAL,
	TEXTURE
};

struct AssetMeta {
	UUID m_uuid = 0;
	virtual AssetType getType() const = 0;
	virtual bool Serialize(const fs::path& assetFile) = 0;
	virtual ~AssetMeta() = default;
};
