#pragma once
#include "TerrainMap.h"
#include <algorithm>
#include "../../../thirdParty/MyGAL/Vector2.h"
// #include "Renderer.h"
namespace MapGenerator
{
	TerrainMap::TerrainMap(const char *name, const std::vector<double> &data, unsigned width, unsigned height)
		: MapComponent(width, height, name)
	{
		/*	m_terrainTypes.push_back(TerrainType(0.5, sf::Color::Blue, "Water"));
			m_terrainTypes.push_back(TerrainType(0.85, sf::Color::Green, "Grass"));
			m_terrainTypes.push_back(TerrainType(1.0, sf::Color::Yellow, "Rock"));*/
		RegenerateTerrain(data);
	}

	TerrainMap::TerrainMap(const char *name, const std::vector<double> &data, unsigned width, unsigned height, const std::vector<TerrainType> &terrain)
		: MapComponent(width, height, name), m_terrainTypes(terrain)
	{
		RegenerateTerrain(data);
	}

	void TerrainMap::RegenerateTerrain(const std::vector<double> &data)
	{
		// rend::drawBuffer(CreateBuffer(data), m_texture, this->Width(), this->Height());
	}

	std::vector<uint8_t> TerrainMap::CreateBuffer(const std::vector<double> &data) const
	{
		unsigned width = this->Width();
		unsigned height = this->Height();
		std::vector<uint8_t> image(width * height * 4);
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				double value = data[width * y + x];

				//sf:: color = GetColorFromHeight(m_terrainTypes, data[width * y + x]);

				//image[4 * width * y + 4 * x + 0] = color.r;
				//image[4 * width * y + 4 * x + 1] = color.g;
				//image[4 * width * y + 4 * x + 2] = color.b;
				//image[4 * width * y + 4 * x + 3] = 255;
			}
		}
		return image;
	}

	data::Color TerrainMap::GetColorFromHeight(const std::vector<TerrainType> &terrainTypes, double height) const
	{
	/*	if (terrainTypes.empty())
			return sf::Color::Black;

		for (auto ter : terrainTypes)
		{
			if (height <= ter.minHeight)
				return ter.color;
		}*/

		//return terrainTypes.back().color;
		return data::Color();
	}

}