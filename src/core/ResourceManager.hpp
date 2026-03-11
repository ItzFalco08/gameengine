#include <unordered_map>
#include <string>
#include <memory>
#include "./Texture.hpp"

class ResourceManager {
public:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

    Texture* LoadAndGetTexture(const std::string& texturePath) {
        if (textures.find(texturePath) == textures.end()) {
            textures[texturePath] = std::make_unique<Texture>(texturePath.c_str());
        }

        return textures[texturePath].get();
    }

    Texture* GetTexture(const std::string& texpath) {
        if (texpath == "") return nullptr;
        if (textures.find(texpath) == textures.end()) return nullptr;
        return textures[texpath].get();
    }

    void DeleteTexture(const std::string& texturePath) {
        if (textures.find(texturePath) != textures.end()) {
            textures.erase(texturePath);
        }
    }

};