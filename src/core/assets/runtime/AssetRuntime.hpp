#pragma once
#include "../../../utils/Utils.hpp"
#include "../meta/AssetMeta.hpp"

struct RuntimeAsset {
	UUID m_uuid{0};

	virtual AssetType getType() const = 0;
	virtual bool Serialize(const AssetMeta& entry) = 0;
	virtual ~RuntimeAsset() = default;
};