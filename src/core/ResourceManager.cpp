#include "ResourceManager.hpp"

Texture2D* ResourceManager::LoadTexture(const std::string& texturePath, TexDets dets) {
    if (textures.find(texturePath) == textures.end()) {
        textures[texturePath] = std::make_unique<Texture2D>(texturePath.c_str(), dets);
    }

    return dynamic_cast<Texture2D*>(textures[texturePath].get());
}

Cubemap* ResourceManager::LoadCubemap(std::vector<std::string> texturePaths, TexDets dets) {
    return nullptr; // future implementation
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