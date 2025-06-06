#pragma once
#include <vector>
#include "../MapComponent.h"
#include <cassert>
#include <algorithm>
#include <memory>
#include "../MapSpecs.h"
class FastNoiseLite;
namespace MapGenerator
{
	// class Texture;
	class HeightMap : public MapComponent
	{
	public:
		HeightMap(const char *name, const NoiseMapData &data);
		HeightMap(const char *name, unsigned width, unsigned height, std::vector<double> &&elevation);
		// HeightMap(const Texture& texture, double noiseScale, const siv::PerlinNoise& noise, const NoiseSpecs& specs);
		~HeightMap();

		void RegenerateHeightMap(const NoiseMapData &data);
		inline std::vector<double> NoiseMap() const
		{
			return m_elevation;
		}

		void SetNoiseMap(std::vector<double> &&elevation);

	private:
		std::vector<double> CreateHeightMapFromNoise(const NoiseMapData &data) const;
		void CreateHeightTectonicPlates();
		void CreateHeightTectonicPlates(const NoiseMapData &data);
		std::vector<uint8_t> CreateBuffer();

		std::vector<double> m_elevation;

		// double m_noiseScale;
		// FastNoiseLite m_noise;
		// NoiseSpecs m_noiseSpecs;
	};

}