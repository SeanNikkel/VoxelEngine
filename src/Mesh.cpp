#include "Mesh.h"

#include <algorithm>

const Vertex Mesh::quads[Math::DIRECTION_COUNT][Math::CORNER_COUNT] =
{
	// QUAD_LEFT
	{
		{ { 0.0f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0 },
		{ { 0.0f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0 },
		{ { 0.0f,  0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, 0 },
		{ { 0.0f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, 0 },
	},

	// QUAD_RIGHT
	{
		{ { 0.0f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, 0 },
		{ { 0.0f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, 0 },
		{ { 0.0f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, 0 },
		{ { 0.0f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, 0 },
	},

	// QUAD_TOP
	{
		{ {  0.5f, 0.0f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 0 },
		{ { -0.5f, 0.0f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 0 },
		{ {  0.5f, 0.0f,  0.5f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, 0 },
		{ { -0.5f, 0.0f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, 0 },
	},  

	// QUAD_BOTTOM
	{
		{ {  0.5f, 0.0f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, 0 },
		{ { -0.5f, 0.0f,  0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, 0 },
		{ {  0.5f, 0.0f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, 0 },
		{ { -0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, 0 },
	},

	// QUAD_FRONT
	{
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0 },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0 },
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 0 },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 0 },
	},

	// QUAD_BACK
	{
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, 0 },
		{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, 0 },
		{ {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, 0 },
		{ { -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, 0 },
	},
};

const unsigned Mesh::quadIndices[] = { 0, 1, 2, 2, 1, 3 };

Mesh::Mesh(unsigned reserve)
{
	SetupObjects();
	vertices_.reserve(reserve);
}

//Mesh::Mesh(Vertex vertices[], unsigned vertexCount)
//{
//	SetupObjects();
//	SetVertices(vertices, vertexCount);
//}
//
//void Mesh::AddVertex(const Vertex &vertex)
//{
//	vertices_.insert(vertices_.end(), vertex);
//	dirty_ = true;
//}

void Mesh::AddQuad(Math::Direction orientation, glm::vec3 offset, float uvScale, glm::vec2 uvOffset, unsigned char ambients[])
{
	vertices_.insert(vertices_.end(), &quads[orientation][0], &quads[orientation][Math::CORNER_COUNT]);
	dirty_ = true;
	
	for (unsigned i = 0; i < Math::CORNER_COUNT; i++)
	{
		unsigned current = vertices_.size() - (Math::CORNER_COUNT - i);
		vertices_[current].position += offset;

		vertices_[current].uv *= uvScale;
		vertices_[current].uv += uvOffset;

		if (ambients != nullptr)
			vertices_[current].ambient = ambients[i];
	}

	if (ambients != nullptr)
	{
		if (ambients[0] + ambients[3] > ambients[1] + ambients[2])
		{
			// Flip quad
			std::swap(*(vertices_.end() - 4), *(vertices_.end() - 2));
			std::swap(*(vertices_.end() - 3), *(vertices_.end() - 1));
			std::swap(*(vertices_.end() - 3), *(vertices_.end() - 2));
		}
	}

	// This needs to be changed if meshes support non-quads
	unsigned last;
	if (indices_.size() == 0)
		last = 0;
	else
		last = *(indices_.end() - 1) + 1;
	indices_.insert(indices_.end(), quadIndices, quadIndices + _countof(quadIndices));
	std::transform(indices_.end() - _countof(quadIndices), indices_.end(), indices_.end() - _countof(quadIndices), [last](unsigned current) { return current + last; });
}

//void Mesh::SetVertices(Vertex vertices[], unsigned vertexCount)
//{
//	Clear();
//	vertices_.insert(vertices_.end(), vertices, &vertices[vertexCount]);
//}

void Mesh::Clear()
{
	vertices_.clear();
	indices_.clear();
	dirty_ = true;
}

unsigned Mesh::VertexCount() const
{
	return vertices_.size();
}

void Mesh::Draw()
{
	if (dirty_)
		UpdateBuffers();

	glBindVertexArray(vao_);

	//glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
}

void Mesh::SetupObjects()
{
	// VBO

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	// VAO

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	// uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	// normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	// ambient
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, ambient));
	glEnableVertexAttribArray(3);

	// EBO

	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::UpdateBuffers()
{
	dirty_ = false;

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(*vertices_.data()), vertices_.data(), GL_DYNAMIC_DRAW); // TODO: profile GL_DYNAMIC_DRAW
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(*indices_.data()), indices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
