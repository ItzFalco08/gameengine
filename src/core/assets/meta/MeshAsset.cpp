#include "MeshAsset.hpp"
#include <fstream>
#include "../../../utils/Logger.hpp"
#include <json/json.hpp>

bool MeshAsset::Serialize(const fs::path& assetFile) {
	try {
		m_sourceFile = assetFile.parent_path() / assetFile.stem();
		if (!fs::exists(m_sourceFile)) {
			return false;
		}
		nlohmann::json assetFileJson;
		std::ifstream(assetFile) >> assetFileJson; 
		m_uuid = UUID::fromString(assetFileJson.at("uuid").get<std::string>());
		m_path = assetFile;
		m_vertexCount = assetFileJson.at("vertexCount").get<uint32_t>();
		m_indexCount = assetFileJson.at("indexCount").get<uint32_t>();
		m_submeshCount = assetFileJson.at("submeshCount").get<uint32_t>();
		return true;
	} catch (const std::exception& e) {
		LOG::Error("Failed to serialize MeshAsset: ", e.what());
		return false;
	}
}