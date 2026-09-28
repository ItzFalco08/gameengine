#include "MeshAsset.hpp"
#include <fstream>

bool MeshAsset::Serialize(const fs::path& assetFile) {
	sourceFile = assetFile.parent_path() / assetFile.stem();
	if (!fs::exists(sourceFile)) {
		return false;
	}
	std::ifstream(assetFile).read(reinterpret_cast<char*>(&m_uuid), sizeof(UUID));

	return true;
}
