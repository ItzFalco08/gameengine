
#pragma once
#include <optional>
#include <vector>
#include "Component.hpp"
#include "../../utils/Utils.hpp"
#include "json/json.hpp"

class MeshComponent : public Component {
public:
    GLuint VAO, VBO, EBO; 
    std::optional<std::string> objFilePath;
    GLenum cullDir = GL_BACK; 
    int indexCount;

    static const char* StaticType() { return "Mesh"; }
    std::string GetType() override { return StaticType(); }
    MeshComponent();
    MeshComponent(const char* objPath);
    bool Initialize(const char* objPath);
    MeshComponent(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    void Serialize(nlohmann::json& json) {
        json["filePath"] = objFilePath.has_value() ? objFilePath.value() : "";
        json["cullDir"] = (unsigned int)cullDir;
    }

    void Deserialize(nlohmann::json& json) {
        cullDir = json.at("cullDir");
        Initialize(json.at("filePath").get<std::string>().c_str());
    }

    ~MeshComponent() override;


private:
    void uploadVertices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};