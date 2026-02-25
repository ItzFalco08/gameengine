#pragma once
#include <optional>
#include "Component.hpp"
#include "../../utils/Utils.hpp"
#include "json/json.hpp"

enum CullDir {
    FRONT,
    BACK,
    BOTH
};

class Mesh : public Component {
public:
    unsigned int VAO, VBO; // used while rendering
    std::optional<std::string> objFilePath;
    CullDir cullDir = FRONT; // read and set by renderer

    size_t GetId() override;
    Mesh();
    Mesh(const char* objPath);
    bool Initialize(const char* objPath);
    Mesh(const std::vector<Vertex>&& vertices); // sensable for scripts [no need of serialization in that case :) ]

    void Serialize(nlohmann::json& json) {
        json["filePath"] = objFilePath.has_value() ? objFilePath.value() : "";
        json["cullDir"] = cullDir;
    }

    void Deserialize(nlohmann::json& json) {
        Initialize(json.at("filePath").get<std::string>().c_str());
    }

    ~Mesh() override;
    std::string GetType() override;


private:
    void uploadVertices(const std::vector<Vertex>& vertices);
};