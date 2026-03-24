#include "Mesh.hpp"
#include <typeinfo>
#include "../../utils/Logger.hpp"
#include "objloader/simpleobjloader.hpp"

Mesh::Mesh(const char* objPath) {
    Initialize(objPath);
}

Mesh::Mesh() : VAO(GL_NONE), VBO(GL_NONE), EBO(GL_NONE) {};

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    uploadVertices(vertices, indices);
}

Mesh::~Mesh() {
}

void Mesh::uploadVertices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    
    glBindVertexArray(this->VAO);

    // V B O
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // E B O 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // A T T R I B U T E S
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, v));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vn));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vt));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


bool Mesh::Initialize(const char* objPath) {
    if (objPath == objFilePath) {LOG::Warning("Mesh with path Already Added: ", objPath); return false;};
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    loadObj(objPath, vertices, indices);
    uploadVertices(vertices, indices);
    indexCount = (int)indices.size();
    LOG::Info("Mesh Created");
    objFilePath = objPath;
    return true;
}
