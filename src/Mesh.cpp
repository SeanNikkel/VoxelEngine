#include "Mesh.h"

#include <algorithm>

// Convenient quad vertices
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

// Indices for above vertices
const unsigned Mesh::quadIndices[] = { 0, 1, 2, 2, 1, 3 };

Mesh::Mesh(unsigned reserve) : reserveAmount_(reserve)
{
	SetupObjects();
}

void Mesh::AddQuad(Math::Direction orientation, glm::vec3 offset, float uvScale, glm::vec2 uvOffset, unsigned char ambients[])
{
	// Reserve for performance
	if (vertices_.capacity() == 0)
	{
		vertices_.reserve(reserveAmount_);
		indices_.reserve(reserveAmount_ * 3 / 2);
	}

	// Insert base quad
	vertices_.insert(vertices_.end(), &quads[orientation][0], &quads[orientation][Math::CORNER_COUNT]);
	onCpu_ = true;
	
	// Transform quad data by parameters given
	for (size_t i = 0; i < Math::CORNER_COUNT; i++)
	{
		size_t current = vertices_.size() - (Math::CORNER_COUNT - i);
		vertices_[current].position += offset;

		vertices_[current].uv *= uvScale;
		vertices_[current].uv += uvOffset;

		if (ambients != nullptr)
			vertices_[current].ambient = ambients[i];
	}

	// Flip quad if the ambient data needs a flipped quad to interpolate correctly
	if (ambients != nullptr && ambients[0] + ambients[3] > ambients[1] + ambients[2])
	{
		std::swap(*(vertices_.end() - 4), *(vertices_.end() - 2));
		std::swap(*(vertices_.end() - 3), *(vertices_.end() - 1));
		std::swap(*(vertices_.end() - 3), *(vertices_.end() - 2));
	}

	// Get next index to use (1 + last one in list)
	unsigned last = 0;
	if (indices_.size() != 0)
		last = *(indices_.end() - 1) + 1;

	// Insert base quad indices
	indices_.insert(
		indices_.end(),
		quadIndices,
		quadIndices + std::size(quadIndices)
	);

	// Add the index offset to the base indices
	std::transform(
		indices_.end() - std::size(quadIndices),
		indices_.end(),
		indices_.end() - std::size(quadIndices),
		[last](unsigned current) { return current + last; }
	);
	indexCount_ += GLsizei(std::size(quadIndices));
}

void Mesh::SetVertices(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
{
	vertices_ = vertices;
	indices_ = indices;
	indexCount_ = GLsizei(indices_.size());
}

void Mesh::Clear()
{
	vertices_.clear();
	indices_.clear();
	indexCount_ = 0;
	onCpu_ = true;
}

unsigned Mesh::IndexCount() const
{
	return indexCount_;
}

bool Mesh::OnCPU() const
{
	return onCpu_;
}

void Mesh::Draw()
{
	// Mesh must be on gpu to draw
	if (onCpu_)
		TransferToGPU();

	glBindVertexArray(vao_);

	// Draw
	glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);

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

void Mesh::TransferToGPU()
{
	if (onCpu_)
	{
		onCpu_ = false;

		// Send VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(*vertices_.data()), vertices_.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Remove cpu data
		vertices_.clear();
		vertices_.shrink_to_fit();

		// Send EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(*indices_.data()), indices_.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Remove cpu data
		indices_.clear();
		indices_.shrink_to_fit();
	}
}
