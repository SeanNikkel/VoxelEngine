#include "Math.h"
#include "Mesh.h"

Math::Direction Math::AxisToDir(Axis axis, bool negative)
{
	return Math::Direction(axis * 2 + (negative ? 1 : 0));
}

Math::Direction Math::VectorToDir(glm::vec3 vec)
{
	float max = vec[0];
	int axis = 0;
	for (int i = 1; i < AXIS_COUNT; i++)
	{
		float val = glm::abs(vec[i]);
		if (val > max)
		{
			max = val;
			axis = i;
		}
	}
	return AxisToDir(Axis(axis), vec[axis] < 0.0f);
}

glm::vec3 Math::CornerToVec(Corner corner, Direction normal)
{
	return Mesh::quads[normal][corner].position * 2.f;
}

float Math::PositiveMod(float val, float mod)
{
	val = fmodf(val, mod);

	if (val < 0)
		val += mod;

	return val;
}

int Math::PositiveMod(int val, int mod)
{
	if (val >= 0)
		return val % mod;

	return (mod * ((-val / mod) + 1)) + val - 1;
}

glm::vec2 Math::GetUVFromSheet(unsigned sizeX, unsigned sizeY, unsigned index, Corner corner)
{
	unsigned column = index % sizeX;
	unsigned row = index / sizeY;

	glm::vec2 topLeft = { column / (float)sizeX,
						  row / (float)sizeY };

	switch (corner)
	{
	case CORNER_TOP_LEFT:
		return topLeft;
	case CORNER_TOP_RIGHT:
		return { topLeft.x + 1, topLeft.y };
	case CORNER_BOTTOM_LEFT:
		return { topLeft.x, topLeft.y + 1 };
	case CORNER_BOTTOM_RIGHT:
		return { topLeft.x + 1, topLeft.y + 1 };
	default:
		return { 0.0f, 0.0f };
	}
}

Math::Frustum Math::CalculateFrustum(const glm::mat4 &camera)
{
	Frustum result;
	glm::vec4 base = { camera[0][3], camera[1][3], camera[2][3], camera[3][3] };

	for (unsigned i = 0; i < DIRECTION_COUNT; i++)
	{
		glm::vec4 normal = { camera[0][i /2], camera[1][i /2], camera[2][i /2], camera[3][i /2] };

		if (i % 2 == 1)
			normal = -1.0f * normal;

		normal += base;
		glm::vec3 eq = normal;
		float len = glm::length(eq);
		eq /= len;
		result.planes[i] = { eq.x, eq.y, eq.z, normal.w / len };
	}

	return result;
}

bool Math::AABBCollision(glm::vec3 pos0, glm::vec3 size0, glm::vec3 pos1, glm::vec3 size1)
{
	return pos0.x - size0.x / 2.f < pos1.x + size1.x / 2.f && pos0.x + size0.x / 2.f > pos1.x - size1.x / 2.f &&
		   pos0.y - size0.y / 2.f < pos1.y + size1.y / 2.f && pos0.y + size0.y / 2.f > pos1.y - size1.y / 2.f &&
		   pos0.z - size0.z / 2.f < pos1.z + size1.z / 2.f && pos0.z + size0.z / 2.f > pos1.z - size1.z / 2.f;
}

float Math::DistToBlock(glm::vec3 pos, Axis axis, glm::vec3 dir)
{
	if (dir[axis] == 0.0f)
		return INFINITY;
	else
		return DistToBlock(pos, axis, dir[axis] < 0.0f);
}

float Math::DistToBlock(glm::vec3 pos, Axis axis, bool negative)
{
	return DistToBlock(pos[axis], axis, negative);
}

float Math::DistToBlock(float pos, Axis axis, bool negative)
{
	float result;

	if (!negative)
		result = 1.0f - glm::fract(pos);
	else
		result = glm::fract(pos);
	if (result == 0.0f)
		result = 1.0f;

	return result;
}
