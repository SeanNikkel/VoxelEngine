#pragma once

namespace World
{
#ifdef _DEBUG
	const unsigned renderDistance = 2; // chunk render radius
	const unsigned renderSpeed = 1; // chunks generated per frame
#else
	const unsigned renderDistance = 32; // chunk render radius
	const unsigned renderSpeed = 12; // chunks generated per frame
#endif

	const float gravity = 9.8f;

	const unsigned chunkHeight = 64;
	const unsigned chunkSize = 16;

	const unsigned chunkArea = chunkSize * chunkSize;
	const unsigned chunkVolume = chunkArea * chunkHeight;

	namespace Generation
	{
		// Mountain noise
		const float heightScale = 128.0f;
		const float heightWeight = 0.9f;
		const unsigned heightMaxHeight = 63;

		// Hill noise
		const float detailScale = 16.0f;
		const unsigned detailMaxHeight = 63;
		const unsigned minHeight = 1;

		// Blocks/sample (unused)
		const float lerpGridSize = 2.0f;

		const float detailWeight = 1.0f - heightWeight;
	}
}