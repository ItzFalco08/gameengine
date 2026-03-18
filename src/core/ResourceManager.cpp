#include "ResourceManager.hpp"

Texture* ResourceManager::LoadAndGetTexture(const std::string& texturePath, TexDets dets) {
    if (textures.find(texturePath) == textures.end()) {
        textures[texturePath] = std::make_unique<Texture>(texturePath.c_str(), dets);
    }

    return textures[texturePath].get();
}

Texture* ResourceManager::GetTexture(const std::string& texpath) {
    if (texpath == "") return nullptr;
    if (textures.find(texpath) == textures.end()) return nullptr;
    return textures[texpath].get();
}

void ResourceManager::DeleteTexture(const std::string& texturePath) {
    if (textures.find(texturePath) != textures.end()) {
        textures.erase(texturePath);
    }
}