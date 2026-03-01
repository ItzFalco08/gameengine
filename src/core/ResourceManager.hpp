#include <unordered_map>
#include <string>
#include <memory>
#include "./Texture.hpp"

class ResourceManager {
public:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

    void AddTexture(const char* path) {
        if (!textures[path]) {
            textures[path] = std::make_unique<Texture>();
        }
    }

    
};