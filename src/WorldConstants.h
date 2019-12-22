#pragma once

namespace World
{
#ifdef _DEBUG
	const float renderDistance = 32.0f; // block render radius
	const unsigned renderSpeed = 1; // chunks generated per frame
#else
	const float renderDistance = 600.0f; // block render radius
	const unsigned renderSpeed = 8; // chunks generated per frame
#endif

	const float gravity = 20.0f;

	const unsigned chunkHeight = 256;
	const unsigned chunkSize = 16;

	const float chunkFloatDistance = chunkHeight / 2.f;
	const float chunkFloatSpeed = 1.0f;

	const unsigned chunkArea = chunkSize * chunkSize;
	const unsigned chunkVolume = chunkArea * chunkHeight;

	namespace Generation
	{
		// Mountain noise
		const float heightScale = 128.0f;
		const float heightWeight = 0.8f;
		const unsigned heightMaxHeight = 255;

		// Hill noise
		const float detailScale = 16.0f;
		const unsigned detailMaxHeight = 63;
		const unsigned minHeight = 1;

		// Blocks/sample (unused)
		const float lerpGridSize = 2.0f;

		const float detailWeight = 1.0f - heightWeight;
	}
}