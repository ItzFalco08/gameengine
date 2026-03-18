#pragma once
#include <optional>
#include "Component.hpp"
#include "../../utils/Utils.hpp"
#include "json/json.hpp"

class Mesh : public Component {
public:
    unsigned int VAO, VBO; 
    std::optional<std::string> objFilePath;
    GLenum cullDir = GL_BACK; 
    int vertexCount;

    static const char* StaticType() { return "Mesh"; }
    std::string GetType() override { return StaticType(); }
    Mesh();
    Mesh(const char* objPath);
    bool Initialize(const char* objPath);
    Mesh(const std::vector<Vertex>&& vertices); // sensable for scripts [no need of serialization in that case :) ]

    void Serialize(nlohmann::json& json) {
        json["filePath"] = objFilePath.has_value() ? objFilePath.value() : "";
        json["cullDir"] = (unsigned int)cullDir;
    }

    void Deserialize(nlohmann::json& json) {
        cullDir = json.at("cullDir");
        Initialize(json.at("filePath").get<std::string>().c_str());
    }

    ~Mesh() override;


private:
    void uploadVertices(const std::vector<Vertex>& vertices);
};