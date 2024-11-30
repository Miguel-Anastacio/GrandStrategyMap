#pragma once
#include "../../ThirdParty/fastNoiseLite/FastNoiseLite.h"
namespace MapGenerator
{
	struct LookupFeatures
	{
		int seed = 0;
		int tiles = 5;
		int lloyd = 0;

		LookupFeatures(int s, int pts, int l) : seed(s), tiles(pts), lloyd(l) {};
		LookupFeatures() = default;
	};

	struct NoiseData
	{
		FastNoiseLite::NoiseType noiseType = FastNoiseLite::NoiseType::NoiseType_OpenSimplex2;
		FastNoiseLite::FractalType fractalType = FastNoiseLite::FractalType::FractalType_FBm;
		int seed = 100123;
		int octaves = 5;
		float frequency = 0.01f;
		float scale = 1.f;
		float lacunarity = 2.0f;
		float fractalGain = 0.5f;
		NoiseData(int s, int oct, float f, float scl, float lac, float fractalgain) : seed(s), octaves(oct), frequency(f), scale(scl), lacunarity(lac), fractalGain(fractalgain) {};
		NoiseData() = default;
	};

	struct LookupMapData
	{
		NoiseData borderNoise;
		LookupFeatures land;
		LookupFeatures ocean;
		unsigned width = 10;
		unsigned height = 10;
		float borderLine = 3.0f;
		float cutOffHeight = 0.001f;

		LookupMapData(NoiseData noiseData, LookupFeatures landSpec, LookupFeatures oceanSpec, unsigned w, unsigned h, float borderThick, float cutOff) : borderNoise(noiseData),
																																						 land(landSpec),
																																						 ocean(oceanSpec),
																																						 width(w), height(h),
																																						 borderLine(borderThick),
																																						 cutOffHeight(cutOff) {};
		LookupMapData() = default;
	};

	static void ApplyNoiseData(const NoiseData &data, FastNoiseLite &noise)
	{
		noise.SetSeed(data.seed);
		noise.SetNoiseType(data.noiseType);
		noise.SetFrequency(data.frequency);
		noise.SetFractalOctaves(data.octaves);
		noise.SetFractalType(data.fractalType);
		noise.SetFractalLacunarity(data.lacunarity);
		noise.SetFractalGain(data.fractalGain);
	}

	struct NoiseMapData
	{
		unsigned width = 10;
		unsigned height = 10;
		int seed = 100123;
		int octaves = 5;
		float frequency = 0.01f;
		float scale = 1.f;
		float lacunarity = 2.0f;
		NoiseMapData(unsigned w, unsigned h, int s = 100123, int oct = 5, float f = 0.01, float scl = 1.0f, float lac = 2.0) : width(w), height(h), seed(s), octaves(oct), frequency(f), scale(scl), lacunarity(lac) {};
		NoiseMapData() = default;
	};
}