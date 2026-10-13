#include "TextureAsset.hpp"
#include <json/json.hpp>
#include <fstream>

bool TextureAsset::Serialize(const fs::path& assetFile) {
	if (!fs::exists(assetFile) || !fs::is_regular_file(assetFile))
		return false;

	try {
		std::ifstream file(assetFile);
		if (!file)
			return false;

		nlohmann::json j;
		file >> j;
		m_uuid = UUID::fromString(j.at("uuid").get<std::string>());
		m_texturePath = j.at("texturePath").get<fs::path>();
		m_textureType = j.at("textureType").get<std::string>() == "Texture2D" ? TextureType::TEXTURE_2D : TextureType::CUBEMAP;

		m_wrapU = j.at("wrapU").get<GLuint>();
		m_wrapV = j.at("wrapV").get<GLuint>();
		m_minFilter = j.at("minFilter").get<GLuint>();
		m_magFilter = j.at("magFilter").get<GLuint>();

		m_path = assetFile;
	}
	catch(const nlohmann::json::exception&) {
		return false;
	}

	return true;
}
