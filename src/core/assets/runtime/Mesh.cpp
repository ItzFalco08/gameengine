#include "Mesh.hpp"
#include <fstream>
#include "../meta/MeshAsset.hpp"
#include "Mesh.hpp"
#include <cstring>

bool Mesh::Serialize(const AssetMeta& entry) {
	submeshes.clear();
	if (VAO != GL_NONE || VBO != GL_NONE || EBO != GL_NONE) {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
		VAO = VBO = EBO = GL_NONE;
	}

	const MeshAsset& meshMeta = static_cast<const MeshAsset&>(entry);
	std::ifstream file(meshMeta.m_sourceFile, std::ios::binary);
	if (!file.is_open()) return false;

	file.seekg(0, std::ios::end);
	const std::streamsize size = file.tellg();
	if (size < static_cast<std::streamsize>(sizeof(uint32_t) * 2 + sizeof(uint32_t))) return false;
	file.seekg(0, std::ios::beg);

	std::vector<char> data(size);
	if (!file.read(data.data(), size)) return false;

	const char* ptr = data.data();
	std::size_t remaining = data.size();
	auto read = [&ptr, &remaining](void* destination, std::size_t bytes) {
		if (bytes > remaining) return false;
		std::memcpy(destination, ptr, bytes);
		ptr += bytes;
		remaining -= bytes;
		return true;
	};

	uint32_t verticesSize = 0;
	uint32_t indicesSize = 0;
	if (!read(&verticesSize, sizeof(verticesSize)) || !read(&indicesSize, sizeof(indicesSize)) ||
		verticesSize == 0 || indicesSize == 0) return false;
	if (verticesSize > remaining / sizeof(Vertex)) return false;

	std::vector<Vertex> vertices(verticesSize);
	if (!read(vertices.data(), verticesSize * sizeof(Vertex))) return false;
	if (indicesSize > remaining / sizeof(GLuint)) return false;

	std::vector<GLuint> indices(indicesSize);
	if (!read(indices.data(), indicesSize * sizeof(GLuint))) return false;

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

	// location 3: tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint32_t submeshCount = 0;
	if (!read(&submeshCount, sizeof(submeshCount)) || submeshCount > remaining / sizeof(Submesh)) return false;

	for (uint32_t i = 0; i < submeshCount; ++i) {
		Submesh submesh;
		if (!read(&submesh, sizeof(Submesh))) return false;
		if (static_cast<uint64_t>(submesh.indicesOffset) + submesh.indicesSize > indicesSize) return false;
		submeshes.push_back(std::move(submesh));
	}

	return true;
}
