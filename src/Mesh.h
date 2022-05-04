#pragma once

#include <vector>

#include "glad/glad.h"
#include <glm/glm.hpp>

#include "Math.h"

// Vertex layout
struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	GLubyte ambient;
};

// Wrapper for graphics api mesh
class Mesh
{
public:
	Mesh(unsigned reserve = 0);

	// Add a quad manually
	void AddQuad(
		Math::Direction orientation,
		glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f),
		float uvScale = 1.0f,
		glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f),
		unsigned char ambients[] = nullptr
	);

	// Set mesh data
	void SetVertices(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);

	// Clear all vertices
	void Clear();

	// Get info
	unsigned IndexCount() const;
	bool OnCPU() const;

	// Clears memory from cpu and readies to draw
	void TransferToGPU();

	// Draws the mesh
	void Draw();

	~Mesh();

	static const Vertex quads[Math::DIRECTION_COUNT][Math::CORNER_COUNT];

	static const unsigned quadIndices[];
private:
	GLuint vbo_;
	GLuint vao_;
	GLuint ebo_;
	std::vector<Vertex> vertices_;
	std::vector<GLuint> indices_;
	GLsizei indexCount_;
	size_t reserveAmount_;

	bool onCpu_ = true;

	void SetupObjects(); // Create initial gpu data
};