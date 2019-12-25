#pragma once

namespace World
{
#ifdef _DEBUG
	const float renderDistance = 32.0f; // block render radius
	const unsigned renderSpeed = 1; // chunks generated per frame
#else
	const float renderDistance = 600.0f; // block render radius
	const unsigned renderSpeed = 2; // chunks generated per frame
#endif

	const float gravity = 20.0f;

	const unsigned chunkHeight = 256;
	const unsigned chunkSize = 16;

	const float chunkFloatDistance = chunkHeight / 2.f;
	const float chunkFloatInSpeed = 1.0f;
	const float chunkFloatOutSpeed = 0.25f;

	const unsigned chunkArea = chunkSize * chunkSize;
	const unsigned chunkVolume = chunkArea * chunkHeight;

	namespace Generation
	{
		const unsigned minHeight = 1;

		// Biome noise
		const float landScale = 2048.0f;

		// Mountain noise
		const float heightScale = 128.0f;
		const float heightWeight = 0.8f;
		const unsigned heightMaxHeight = 255;

		// Hill noise
		const float detailScale = 16.0f;
		const unsigned detailMaxHeight = 63;

		// Trees
		const unsigned minTreeDistance = 4;

		// Interpolation grid size
		const float terrainInterpGrid = 4.0f;

		const float detailWeight = 1.0f - heightWeight;
	}
}