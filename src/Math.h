#pragma once

#include <cmath>
#include <glm/glm.hpp>

namespace Math
{
	enum Axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,

		AXIS_COUNT
	};

	enum Direction
	{
		DIRECTION_LEFT,
		DIRECTION_RIGHT,
		DIRECTION_UP,
		DIRECTION_DOWN,
		DIRECTION_FORWARD,
		DIRECTION_BACKWARD,

		DIRECTION_COUNT
	};

	enum Corner
	{
		CORNER_BOTTOM_LEFT,
		CORNER_BOTTOM_RIGHT,
		CORNER_TOP_LEFT,
		CORNER_TOP_RIGHT,

		CORNER_COUNT
	};

	struct Plane
	{
		float a, b, c, d;
	};

	struct Frustum
	{
		Plane planes[Math::DIRECTION_COUNT];
	};

	const glm::vec3 directionVectors[DIRECTION_COUNT]
	{
		{ 1, 0, 0 },
		{ -1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, -1, 0 },
		{ 0, 0, 1 },
		{ 0, 0, -1 },
	};

	const glm::ivec2 surrounding[] = {
		glm::ivec2(-1.0f,  0.0f),
		glm::ivec2(1.0f,  0.0f),
		glm::ivec2(0.0f, -1.0f),
		glm::ivec2(0.0f,  1.0f),
		glm::ivec2(-1.0f,  1.0f),
		glm::ivec2(1.0f,  1.0f),
		glm::ivec2(1.0f, -1.0f),
		glm::ivec2(-1.0f, -1.0f),
	};

	Direction AxisToDir(Axis axis, bool negative);
	Direction VectorToDir(glm::vec3 vec);
	glm::vec3 CornerToVec(Corner corner, Direction normal);

	float PositiveMod(float val, float mod);
	int PositiveMod(int val, int mod);

	glm::vec2 GetUVFromSheet(unsigned sizeX, unsigned sizeY, unsigned index, Corner corner);

	Frustum CalculateFrustum(const glm::mat4 &camera);

	bool AABBCollision(glm::vec3 pos0, glm::vec3 size0, glm::vec3 pos1, glm::vec3 size1);

	float DistToBlock(glm::vec3 pos, Axis axis, glm::vec3 dir);
	float DistToBlock(glm::vec3 pos, Axis axis, bool negative);
	float DistToBlock(float pos, Axis axis, bool negative);
}