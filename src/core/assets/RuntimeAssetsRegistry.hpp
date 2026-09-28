#pragma once
#include "runtime/AssetRuntime.hpp"
#include <unordered_map>
#include "../../utils/Utils.hpp"
#include <memory>

namespace fs = std::filesystem;

class RuntimeAssetsRegistry {
private:
	std::unordered_map<UUID, std::unique_ptr<RuntimeAsset>, UUIDHasher> m_assetsRuntime;

public:
	RuntimeAsset* getAsset(const UUID& uuid);;
};