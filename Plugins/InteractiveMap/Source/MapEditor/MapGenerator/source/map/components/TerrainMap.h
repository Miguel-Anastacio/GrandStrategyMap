#pragma once
#include <vector>
#include <cassert>
#include <string>
#include "../MapComponent.h"
#include "../MapSpecs.h"
namespace MapGenerator
{
	struct TerrainType
	{
		float minHeight;
		//sf::Color color;
		std::string name;

		//TerrainType(float height = 0.0f, sf::Color col = sf::Color::Black, const std::string &nam = "default") : minHeight(height), color(col), name(nam) {};
	};

	using namespace data;
	// class Texture;
	class TerrainMap : public MapComponent
	{
	public:
		TerrainMap(const char *name, const std::vector<double> &noiseValues, unsigned width, unsigned height);
		TerrainMap(const char *name, const std::vector<double> &noiseValues, unsigned width, unsigned height, const std::vector<TerrainType> &terrains);
		// HeightMap(const Texture& texture, double noiseScale, const siv::PerlinNoise& noise, const NoiseSpecs& specs);

		void RegenerateTerrain(const std::vector<double> &noiseValues);

		inline void AddTerrainType(const TerrainType &terrainT)
		{
			m_terrainTypes.push_back(terrainT);
		}

	private:
		// std::vector<double> CreateHeightMap(const NoiseMapData& data) const;
		std::vector<uint8_t> CreateBuffer(const std::vector<double> &data) const;
		data::Color GetColorFromHeight(const std::vector<TerrainType> &colors, double height) const;

		std::vector<TerrainType> m_terrainTypes;
	};

}