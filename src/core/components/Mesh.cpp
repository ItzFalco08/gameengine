#include "Mesh.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

Mesh::Mesh(const char* objPath) {
    Initialize(objPath);
}

Mesh::Mesh() : VAO(GL_NONE), VBO(GL_NONE) {};

Mesh::Mesh(const std::vector<Vertex>&& vertices) {
    uploadVertices(vertices);
}

Mesh::~Mesh() {
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


bool Mesh::Initialize(const char* objPath) {
    if (objPath == objFilePath) {LOG::Warning("Mesh with path Already Added: ", objPath); return false;};
    std::vector<Vertex> vertices;
    if(!Utils::loadObj(vertices, objPath)) return false;
    uploadVertices(vertices);
    vertexCount = (int)vertices.size();
    LOG::Info("Mesh Created");
    objFilePath = objPath;
    return true;
}
