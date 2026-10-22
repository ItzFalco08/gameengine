#include "RuntimeAssetsRegistry.hpp"
#include "meta/TextureAsset.hpp"
#include "runtime/Mesh.hpp"
#include <memory>
#include <utility>
#include "runtime/Texture.hpp"
#include "../../utils/UUID.hpp"
#include "../../utils/globals.hpp"

template<typename T>
T* RuntimeAssetsRegistry::getAsset(const UUID& uuid) {
    if (uuid.low == 0 && uuid.high == 0) {
        return nullptr;
    }

    // Return the cached runtime asset if it already exists.
    if (m_assetsRuntime.has(uuid)) {
        return dynamic_cast<T*>(m_assetsRuntime.get(uuid));
    }

    // Find the asset metadata.
    AssetMeta* meta = engine.assetsRegistry.getAsset<AssetMeta>(uuid);

    if (!meta) {
        LOG::Error("Asset with UUID: ", UUID::toString(uuid), " not found in AssetsRegistry.");
        return nullptr;
    }

    switch (meta->getType()) {
    case AssetType::TEXTURE: {
        auto* texMeta = reinterpret_cast<TextureAsset*>(meta);

        switch (texMeta->m_textureType) {
        case TextureType::TEXTURE_2D: {
            auto texture = std::make_unique<Texture2D>();

            if (!texture->Serialize(*meta)) {
                LOG::Error("Failed to serialize Texture2D with UUID: ", UUID::toString(uuid));
                return nullptr;
            }

            m_assetsRuntime.set(uuid, std::move(texture));
            break;
        }

        case TextureType::CUBEMAP: {
            auto texture = std::make_unique<Cubemap>();

            if (!texture->Serialize(*meta)) {
                LOG::Error("Failed to serialize Cubemap with UUID: ", UUID::toString(uuid));
                return nullptr;
            }

            m_assetsRuntime.set(uuid, std::move(texture));
            break;
        }

        default:
            LOG::Error("Unsupported texture type for UUID: ", UUID::toString(uuid));
            return nullptr;
        }

        break;
    }

    case AssetType::MESH: {
        auto mesh = std::make_unique<Mesh>();

        if (!mesh->Serialize(*meta)) {
            LOG::Error("Failed to serialize Mesh with UUID: ", UUID::toString(uuid));
            return nullptr;
        }

        m_assetsRuntime.set(uuid, std::move(mesh));
        break;
    }

    default:
        LOG::Error("Unsupported asset type for UUID: ", UUID::toString(uuid));
        return nullptr;
    }

    return dynamic_cast<T*>(m_assetsRuntime.get(uuid));
}

// Explicit instantiations for the types used across translation units.
template Mesh* RuntimeAssetsRegistry::getAsset<Mesh>(const UUID&);
template Texture* RuntimeAssetsRegistry::getAsset<Texture>(const UUID&);
