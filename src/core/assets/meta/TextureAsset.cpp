#include "TextureAsset.hpp"
#include <json/json.hpp>

inline bool TextureAsset::Serialize(const fs::path& assetFile) {
	try {
		nlohmann::json j(assetFile);
		m_texturePath = j.at("texturePath").get<fs::path>();
		m_uuid = UUID::fromString(j.at("uuid").get<std::string>());
		m_textureType = j.at("textureType").get<std::string>() == "Texture2D" ? TextureType::TEXTURE_2D : TextureType::CUBEMAP;

		m_wrapU = j.at("wrapU").get<GLuint>();
		m_wrapV = j.at("wrapV").get<GLuint>();
		m_minFilter = j.at("minFilter").get<GLuint>();
		m_magFilter = j.at("magFilter").get<GLuint>();
	}
	catch(const nlohmann::json::exception& e) {
		return false;
	}

	return true;
}
