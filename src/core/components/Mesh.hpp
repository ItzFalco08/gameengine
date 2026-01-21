#pragma once
#include <optional>
#include "Component.hpp"
#include "../../utils/Utils.hpp"
#include "json/json.hpp"


class Mesh : public Component {
public:
    unsigned int VAO, VBO;
    std::optional<std::string> objFilePath;

    size_t GetId() override;
    Mesh();
    Mesh(const char* objPath);
    Mesh(const std::vector<Vertex>&& vertices);

    void Serialize(nlohmann::json& json) {
    }

    void Deserialize(nlohmann::json& json) {
    }

    ~Mesh() override;
    std::string GetType() override;


private:
    void uploadVertices(const std::vector<Vertex>& vertices);
};