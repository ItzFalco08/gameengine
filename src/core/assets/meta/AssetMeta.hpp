#pragma once
#include "../../../utils/UUID.hpp"
#include <filesystem>
namespace fs = std::filesystem;

enum AssetType {
	MESH,
	MATERIAL,
	TEXTURE
};

struct AssetMeta {
	UUID m_uuid = 0;
	fs::path m_path = "";
	virtual AssetType getType() const = 0;
	virtual bool Serialize(const fs::path& assetFile) = 0;
	virtual ~AssetMeta() = default;
};
