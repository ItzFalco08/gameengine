#include "RuntimeAssetsRegistry.hpp"
#include "meta/TextureAsset.hpp"
#include "runtime/Texture.hpp"
#include "runtime/Mesh.hpp"
#include "../../utils/globals.hpp"
#include <memory>

RuntimeAsset* RuntimeAssetsRegistry::getAsset(const UUID& uuid) {
	if (m_assetsRuntime.find(uuid) != m_assetsRuntime.end()) {
		AssetMeta* meta = engine.assetsRegistry.getAsset(uuid);
		if(!meta) {
			LOG::Error("Asset with UUID: ", uuid, " not found in AssetsRegistry.");
			return nullptr;
		}

		switch (meta->getType()) {
		case AssetType::TEXTURE: {
			switch (reinterpret_cast<TextureAsset*>(meta)->m_textureType) {
				case TextureType::TEXTURE_2D: {
					std::unique_ptr<Texture2D> texture = std::make_unique<Texture2D>();
					if(!texture->Serialize(*meta)) {
						LOG::Error("Failed to serialize Texture2D with UUID: ", uuid);
						return nullptr;
					}
					m_assetsRuntime[uuid] = std::move(texture);
					break;
				}
				case TextureType::CUBEMAP: {
					std::unique_ptr<Cubemap> texture = std::make_unique<Cubemap>();
					if(!texture->Serialize(*meta)) {
						LOG::Error("Failed to serialize Cubemap with UUID: ", uuid);
						return nullptr;
					}
					m_assetsRuntime[uuid] = std::move(texture);
					break;
				}
			}

			break;
		}
			
		case AssetType::MESH:
			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
			if(!mesh->Serialize(*meta)) {
				LOG::Error("Failed to serialize Mesh with UUID: ", uuid);
				return nullptr;
			}
			m_assetsRuntime[uuid] = std::move(mesh);
			break;
		}

	}

	return m_assetsRuntime[uuid].get();
}