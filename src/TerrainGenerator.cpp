#include "TerrainGenerator.h"
#include "WorldConstants.h"

#include <glm/gtc/noise.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>

#include <random>

namespace Gen = World::Generation;

// Hardcoded tree data; in the future, this will go in an external data file
const int TerrainGenerator::tree[10][7][7] = {
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 4, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 4, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 4, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 5, 5, 5, 5, 5, 0 },{ 5, 5, 5, 4, 5, 5, 5 },{ 0, 5, 5, 5, 5, 5, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 0, 5, 4, 5, 0, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 5, 5, 5, 5, 5, 0 },{ 0, 5, 5, 4, 5, 5, 0 },{ 0, 5, 5, 5, 5, 5, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 5, 4, 5, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 5, 5, 4, 5, 5, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 5, 5, 5, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
	{{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 5, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 },{ 0, 0, 0, 0, 0, 0, 0 }},
};

int TerrainGenerator::GetHeight(glm::vec2 pos)
{
	// Get grid corners for interpolation
	glm::vec2 scaled = pos / Gen::terrainInterpGrid;
	glm::vec2 min = glm::floor(scaled) * Gen::terrainInterpGrid;
	glm::vec2 max = glm::ceil(scaled) * Gen::terrainInterpGrid;

	if (min == max)
		return static_cast<int>(GetNoiseHeight(pos));

	// Interpolate x and y
	if (min.x != max.x && min.y != max.y)
	{
		// Get noise at each corner
		float bl = GetNoiseHeight(min);
		float tr = GetNoiseHeight(max);
		float tl = GetNoiseHeight({ min.x, max.y });
		float br = GetNoiseHeight({ max.x, min.y });

		// Interpolation values
		float tx = (pos.x - min.x) / (max.x - min.x);
		float ty = (pos.y - min.y) / (max.y - min.y);

		// Lerp y
		float ml = glm::lerp(bl, tl, ty);
		float mr = glm::lerp(br, tr, ty);

		// Lerp x
		return static_cast<int>(glm::lerp(ml, mr, tx));
	}
	
	// Interpolate one dimension
	float minH = GetNoiseHeight(min);
	float maxH = GetNoiseHeight(max);

	if (min.x == max.x) // Interpolate y
		return static_cast<int>(glm::lerp(minH, maxH, (pos.y - min.y) / (max.y - min.y)));
	else // Interpolate x
		return static_cast<int>(glm::lerp(minH, maxH, (pos.x - min.x) / (max.x - min.x)));
}

std::vector<glm::ivec2> TerrainGenerator::GenerateTreePoints(glm::ivec2 startCorner, glm::ivec2 endCorner)
{
	std::vector<glm::ivec2> points;
	std::hash<glm::ivec2> hash;
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	// Check each point for if a tree should be generated
	for (int z = startCorner.y; z < endCorner.y; z++)
	{
		for (int x = startCorner.x; x < endCorner.x; x++)
		{
			glm::ivec2 pos = { x, z };
			std::default_random_engine rng(unsigned(hash(pos)));

			// Generate tree if using coord as seed passes
			if (dist(rng) <= Gen::treeDensity)
			{
				points.push_back(pos);
			}
		}
	}

	return points;
}

float TerrainGenerator::GetNoiseHeight(glm::vec2 pos)
{
	// Check cache
	float cache = GetFromCache(pos);
	if (cache > 0.0f)
		return cache;

	// Calculate layered noise
	float height = ((glm::simplex(pos / float(Gen::heightScale)) + 1) / 2) * Gen::heightWeight * Gen::heightMaxHeight +
				   ((glm::simplex(pos / float(Gen::detailScale)) + 1) / 2) * Gen::detailWeight * Gen::detailMaxHeight;

	// Apply biome height scalar
	height *= (glm::clamp(
		(glm::simplex(pos / static_cast<float>(Gen::landScale)) + Gen::landMountainBias * 2.0f) * Gen::landTransitionSharpness,
		-1.0f + Gen::landMinMult * 2.0f,
		1.0f
	) + 1.0f) / 2.0f;

	// Raise height by min
	height += Gen::minHeight;

	// Add and return
	AddToCache(pos, height);
	return height;
}

void TerrainGenerator::AddToCache(glm::vec2 pos, float height)
{
	if (cacheSize_ >= cacheCapacity)
		cacheSize_ = 0; // Reset cache

	// Add to cache
	cache_[cacheSize_] = { height, pos };
	cacheSize_++;
}

float TerrainGenerator::GetFromCache(glm::vec2 pos)
{
	// Search from end
	for (int i = cacheSize_ - 1; i >= 0; i--)
	{
		if (cache_[i].pos == pos)
			return cache_[i].height;
	}
	return 0.0f;
}
