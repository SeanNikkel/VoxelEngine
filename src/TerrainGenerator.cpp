#include "TerrainGenerator.h"
#include "WorldConstants.h"

#include <glm/gtc/noise.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>

#include <random>

namespace Gen = World::Generation;

int TerrainGenerator::GetHeight(glm::vec2 pos)
{
	glm::vec2 scaled = pos / Gen::terrainInterpGrid;
	glm::vec2 min = glm::floor(scaled) * Gen::terrainInterpGrid;
	glm::vec2 max = glm::ceil(scaled) * Gen::terrainInterpGrid;

	if (min == max)
		return static_cast<int>(GetPerlinHeight(pos));

	// interp x and y
	if (min.x != max.x && min.y != max.y)
	{
		float bl = GetPerlinHeight(min);
		float tr = GetPerlinHeight(max);
		float tl = GetPerlinHeight({ min.x, max.y });
		float br = GetPerlinHeight({ max.x, min.y });

		float tx = (pos.x - min.x) / (max.x - min.x);
		float ty = (pos.y - min.y) / (max.y - min.y);

		float ml = glm::lerp(bl, tl, ty);
		float mr = glm::lerp(br, tr, ty);

		return static_cast<int>(glm::lerp(ml, mr, tx));
	}
	
	// 1D interp
	float minH = GetPerlinHeight(min);
	float maxH = GetPerlinHeight(max);

	if (min.x == max.x)
		return static_cast<int>(glm::lerp(minH, maxH, (pos.y - min.y) / (max.y - min.y)));
	else
		return static_cast<int>(glm::lerp(minH, maxH, (pos.x - min.x) / (max.x - min.x)));
}

std::vector<glm::ivec2> TerrainGenerator::GenerateTreePoints(glm::ivec2 chunkCoord)
{
	std::vector<glm::ivec2> points;
	std::default_random_engine rng(std::hash<glm::ivec2>()(chunkCoord));
	std::uniform_int_distribution<int> dist(0, World::chunkSize - 1);

	unsigned attempts = 0;

	while (attempts <= 10)
	{
		glm::ivec2 point = { dist(rng), dist(rng) };

		bool withinDist = false;
		for (unsigned i = 0; i < points.size(); i++)
		{
			glm::ivec2 dif = point - points[i];
			if (dif.x * dif.x + dif.y * dif.y < Gen::minTreeDistance * Gen::minTreeDistance)
			{
				withinDist = true;
				break;
			}
		}

		if (withinDist)
		{
			attempts++;
		}
		else
		{
			attempts = 0;
			points.push_back(point);
		}
	}
	return points;
}

float TerrainGenerator::GetPerlinHeight(glm::vec2 pos)
{
	// Check cache
	float cache = GetFromCache(pos);
	if (cache > 0.0f)
		return cache;

	// Calculate
	float height = ((glm::perlin(pos / static_cast<float>(Gen::heightScale)) + 1) / 2) * Gen::heightWeight * Gen::heightMaxHeight +
				   ((glm::perlin(pos / static_cast<float>(Gen::detailScale)) + 1) / 2) * Gen::detailWeight * Gen::detailMaxHeight;

	height *= (glm::clamp(glm::perlin(pos / static_cast<float>(Gen::landScale)) * 4.0f, -1.0f, 1.0f) + 1.0f) / 2.0f;

	height += Gen::minHeight;

	AddToCache(pos, height);
	return height;
}

void TerrainGenerator::AddToCache(glm::vec2 pos, float height)
{
	if (cacheSize_ >= cacheCapacity)
		cacheSize_ = 0;

	cache_[cacheSize_] = { height, pos };
	cacheSize_++;
}

float TerrainGenerator::GetFromCache(glm::vec2 pos)
{
	// this is crazy
	for (unsigned i = cacheSize_; i--;)
	{
		if (cache_[i].pos == pos)
			return cache_[i].height;
	}
	return 0.0f;
}
