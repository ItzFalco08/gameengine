#include "Mesh.hpp"
#include <fstream>
#include "../meta/MeshAsset.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"

bool Mesh::Serialize(const AssetMeta& entry) {
	if (VAO != GL_NONE || VBO != GL_NONE || EBO != GL_NONE) {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
		VAO = VBO = EBO = GL_NONE;
	}

	const MeshAsset& meshMeta = static_cast<const MeshAsset&>(entry);
	std::ifstream file(meshMeta.sourceFile, std::ios::binary);
	if (!file.is_open()) return false;

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> data(size);
	if (!file.read(data.data(), size)) return false;

	const char* ptr = data.data();

	uint32_t verticesSize, indicesSize = 0;

	memcpy(&verticesSize, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	assert(verticesSize > 0);

	memcpy(&indicesSize, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	assert(indicesSize > 0);

	std::vector<Vertex> vertices(verticesSize);
	std::vector<GLuint> indices(indicesSize);

	memcpy(vertices.data(), ptr, verticesSize * sizeof(Vertex));
	ptr += verticesSize * sizeof(Vertex);

	memcpy(indices.data(), ptr, indicesSize * sizeof(GLuint));
	ptr += indicesSize * sizeof(GLuint);

	// Setup OpenGL buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// The element buffer binding is stored in the VAO, so bind it while the VAO is bound
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// location 0: position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	// location 1: normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// location 2: uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint32_t submeshCount = 0;
	memcpy(&submeshCount, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	for (uint32_t i = 0; i < submeshCount; ++i) {
		Submesh submesh;
		memcpy(&submesh, ptr, sizeof(Submesh));
		ptr += sizeof(Submesh);
		submeshes.push_back(std::move(submesh));
	}

	return true;
}
