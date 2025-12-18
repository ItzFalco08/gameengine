#pragma once
#include "Component.hpp"
#include "../../utils/Utils.hpp"

class Mesh : public Component {
public:
    unsigned int VAO, VBO;

    size_t GetId() override;
    Mesh(const char* objPath);
    Mesh(const std::vector<Vertex>&& vertices);
    ~Mesh() override;

private:
    void uploadVertices(const std::vector<Vertex>& vertices);
};