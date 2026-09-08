#include <unordered_map>
#include <string>
#include <memory>
#include "./Texture.hpp"
#include <string>

class ResourceManager {
public:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

    // api
    Texture2D* LoadTexture(const std::string& texturePath, TexDets dets = TexDets());
    Cubemap* LoadCubemap(const std::vector<std::string> texturePath, TexDets dets  = TexDets());
    Texture* GetTexture(const std::string& texpath);
    void DeleteTexture(const std::string& texturePath);

};