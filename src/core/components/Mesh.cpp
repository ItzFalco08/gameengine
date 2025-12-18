#include "Mesh.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

size_t Mesh::GetId() {
    return typeid(Mesh).hash_code();
}

Mesh::Mesh(const char* objPath) {
    std::vector<Vertex> vertices;
    Utils::loadObj(vertices, objPath);
    uploadVertices(vertices);
    LOG::Info("Mesh Created");
}

Mesh::Mesh(const std::vector<Vertex>&& vertices) {
    uploadVertices(vertices);
    LOG::Info("Mesh Created");
}

Mesh::~Mesh() {
    LOG::Info("Mesh Destroyed");
}

void Mesh::uploadVertices(const std::vector<Vertex>& vertices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // V B O
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // A T T R I B U T E S
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
