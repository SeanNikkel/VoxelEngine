#pragma once

#include <vector>

#include "glad/glad.h"
#include <glm/glm.hpp>

#include "Math.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	GLubyte ambient;
};

class Mesh
{
public:
	Mesh(unsigned reserve = 0);

	void AddQuad(Math::Direction orientation, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f), float uvScale = 1.0f, glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f), unsigned char ambients[] = nullptr);
	void Clear();

	unsigned IndexCount() const;
	bool OnCPU() const;

	void TransferToGPU();
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
	unsigned indexCount_;
	unsigned reserveAmount_;

	bool onCpu_ = false;

	void SetupObjects();
};