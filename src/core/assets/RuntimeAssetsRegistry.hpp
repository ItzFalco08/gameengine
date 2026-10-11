#pragma once

#include "runtime/AssetRuntime.hpp"
#include "../../utils/Utils.hpp"
#include "AssetPool.hpp"

class RuntimeAssetsRegistry {
private:
    AssetsPool<RuntimeAsset> m_assetsRuntime;

public:
    template<typename T>
    T* getAsset(const UUID& uuid);
};