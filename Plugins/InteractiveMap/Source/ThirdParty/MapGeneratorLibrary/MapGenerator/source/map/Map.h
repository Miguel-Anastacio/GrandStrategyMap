#pragma once
#include <memory>
#include "../data/Utils.h"
#include "../data/Color.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include "../../thirdParty/MyGAL/Diagram.h"
#include "MapSpecs.h"
#include "components/HeightMap.h"
#include "components/TerrainMap.h"
#include "components/MapMask.h"
#include "components/LookupMap.h"
namespace MapGenerator
{
	class Texture;
	using namespace data;
	enum class MapModeGen
	{
		FromHeightMap,
		FromMask
	};

	
	class Map : public Dimensions
	{
	public:
		Map(unsigned width, unsigned height);
		~Map();

		const TileMap& GetLookupTileMap() const
		{
			return m_lookupmap->GetTileMap();
		}
	inline const std::vector<double> NoiseMap() const
	{
		assert(m_heightmap != nullptr);
		return m_heightmap->NoiseMap();
	}
		
	void GenerateHeightMap(const NoiseMapData &data);
	void GenerateHeightMapTectonic();
	void GenerateHeightMapTectonic(const NoiseMapData &data);

	void GenerateTerrainMap(const std::vector<double> &noiseMap);
	void GenerateTerrainMap(const std::vector<double> &noiseMap, const std::vector<TerrainType> &types);

	// void SaveLookupMapToFile() const;
	// void SaveLookupMapToFile(const char* filename) const;
	// void SaveHeightMapToFile(const char* filename) const;

	void RegenerateLookUp(const LookupMapData &data, std::function<void(float, std::string_view)> progressCallback = nullptr);
	void RegenerateLookupBorders(const LookupMapData &data);

	void GenerateMap(const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height);
	void GenerateMap(const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height,
						const LookupMapData& data, const MapModeGen mode = MapModeGen::FromHeightMap,
						std::function<void(float, std::string_view)> progressCallback = nullptr);

	void GenerateMapFromHeigthMap(const std::vector<uint8_t> &textureBuffer, float cutOffHeight);
	void GenerateMapFromHeigthMap(const std::vector<uint8_t> &textureBuffer, float cutOffHeight, const LookupMapData& data,
									std::function<void(float, std::string_view)> progressCallback = nullptr);

	void GenerateMapFromMaskTexture(const std::vector<uint8_t> &maskTextureBuffer, float cutOffHeight,
									const LookupMapData& data,
									std::function<void(float, std::string_view)> progressCallback = nullptr);

	void RegenerateMasks(const LookupMapData& data);	
	void RegenerateMasks(const std::vector<uint8_t> &maskTextureBuffer);	
	// void SaveMap(const char* filePath);
	void SaveMap(const std::string &filePath) const;
	void Reset();

	void SetSize(unsigned width, unsigned height);
	void SetLookupTileMap(const std::vector<Tile>& tiles);
	std::vector<Tile> GetTiles() const;

	bool IsValid() const;

private:
	// void SaveMapComponent(MapComponent* component, const char* filePath, const char* message = "map component");
	void SaveMapComponent(MapComponent *component, const std::string &filePath, const char *message = "map component") const;
	void ClearMapComponent(MapComponent *component, const char *message = "map component");

	std::vector<TerrainType> m_terrainTypes;

	std::unique_ptr<LookupMap> m_lookupmap;
	std::unique_ptr<HeightMap> m_heightmap;
	std::unique_ptr<TerrainMap> m_terrainmap;
	std::unique_ptr<MapMask> m_maskmap;
	std::unique_ptr<MapMask> m_landMask;
	std::unique_ptr<MapMask> m_oceanMask;

	float m_cutOffHeight = 0.001f;
	// std::unique_ptr<Texture> m_lookUpTexture;
	uint8_t* GetTileMapOfType(TileType type) const;
};
}