#include "Map.h"
#include "../utils/ComputeGeometry.h"
#include "../../thirdParty/fastNoiseLite/FastNoiseLite.h"
#include "components/HeightMap.h"
namespace MapGenerator
{

	Map::Map(unsigned width, unsigned height)
		: Dimensions(width, height)
	{
	}

	Map::~Map()
	{
		// m_lookUpTexture->WriteTextureToFile();
		// m_maskmap->SaveToFile("untitledmask.png");
		// m_heightmap->SaveToFile("untitledHeight.png");
		// m_terrainmap->SaveToFile("untitledTerrain.png");
	}

	void Map::GenerateHeightMap(const NoiseMapData &data)
	{
		m_heightmap = std::make_unique<HeightMap>("heightMap1.png", data);
		m_terrainmap = std::make_unique<TerrainMap>("terrainMap.png", m_heightmap->NoiseMap(), data.width, data.height, m_terrainTypes);
	}

	void Map::GenerateHeightMapTectonic()
	{
		m_heightmap = std::make_unique<HeightMap>("heightMap1.png", NoiseMapData());
	}

	void Map::GenerateHeightMapTectonic(const NoiseMapData &data)
	{
		m_heightmap = std::make_unique<HeightMap>("heightMap1.png", data);
	}

	void Map::GenerateTerrainMap(const std::vector<double> &noiseMap)
	{
		m_terrainmap = std::make_unique<TerrainMap>("terrainMap.png", m_heightmap->NoiseMap(), m_heightmap->Width(), m_heightmap->Height(), m_terrainTypes);
	}

	void Map::GenerateTerrainMap(const std::vector<double> &noiseMap, const std::vector<TerrainType> &types)
	{
		m_terrainTypes = types;
		m_terrainmap = std::make_unique<TerrainMap>("terrainMap.png", m_heightmap->NoiseMap(), m_heightmap->Width(), m_heightmap->Height(), types);
	}

	void Map::RegenerateLookUp(const LookupMapData &data, std::function<void(float, std::string_view)> progressCallback)
	{
		assert(m_lookupmap != nullptr);
		assert(m_landMask != nullptr);
		assert(m_oceanMask != nullptr);
		
		if(progressCallback)
		{
			progressCallback(10.0f, "Generated Land and Ocean Masks");
		}
		
		m_lookupmap->RegenerateLookUp(data, m_landMask.get(), m_oceanMask.get(), progressCallback);
		m_cutOffHeight = data.cutOffHeight;
	}

	void Map::RegenerateLookupBorders(const LookupMapData &data)
	{
		assert(m_lookupmap != nullptr);
		assert(m_landMask != nullptr);
		assert(m_oceanMask != nullptr);

		m_lookupmap->RegenerateBorders(data, m_landMask.get(), m_oceanMask.get());
	}

	void Map::GenerateMap(const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height)
	{
		setDimensions(width, height);
		GenerateMapFromHeigthMap(textureBuffer, m_cutOffHeight);
	}

	void Map::GenerateMap(const std::vector<uint8_t>& textureBuffer, unsigned width, unsigned height,
		const LookupMapData& data, const MapModeGen mode,
		std::function<void(float, std::string_view)> progressCallback)
	{
		setDimensions(width, height);
		switch (mode)
		{
		case MapModeGen::FromHeightMap:
			GenerateMapFromHeigthMap(textureBuffer, data.cutOffHeight, data, progressCallback);
			break;
		case MapModeGen::FromMask:
			GenerateMapFromMaskTexture(textureBuffer, data.cutOffHeight, data, progressCallback);
			break;
		}
	}

	void Map::GenerateMapFromHeigthMap(const std::vector<uint8_t> &textureBuffer, float cutOffHeight)
	{
		cutOffHeight = 0.001f;
		// m_maskmap = std::make_unique<MapMask>("LandmassMaskTest.png", textureBuffer, Width(), Height(), cutOffHeight);
		m_landMask = std::make_unique<MapMask>("landMask.png", textureBuffer, Width(), Height(), cutOffHeight);
		m_oceanMask = std::make_unique<MapMask>("oceamMask.png", textureBuffer, Width(), Height(), cutOffHeight, false);
		RegenerateMasks(LookupMapData(NoiseData(), LookupFeatures(), LookupFeatures(), Width(), Height(), 1.0f, cutOffHeight));
		
		m_lookupmap = std::make_unique<LookupMap>("lookupTexture.png", Width(), Height());
		RegenerateLookUp(LookupMapData(NoiseData(), LookupFeatures(), LookupFeatures(), Width(), Height(), 1.0f, cutOffHeight));

		m_heightmap = std::make_unique<HeightMap>("heightMap1.png", Width(), Height(), m_landMask->GetElevation());
		m_terrainmap = std::make_unique<TerrainMap>("terrainMap.png", m_heightmap->NoiseMap(), Width(), Height(), m_terrainTypes);
	}

	void Map::GenerateMapFromHeigthMap(const std::vector<uint8_t>& textureBuffer, float cutOffHeight,
		const LookupMapData& data,std::function<void(float, std::string_view)> progressCallback)
	{
		// cutOffHeight = 0.001f;
		if(progressCallback)
		{
			progressCallback(0.0f, "GenerateMapFromHeigthMap");
		}
		
		// m_maskmap = std::make_unique<MapMask>("LandmassMaskTest.png", textureBuffer, Width(), Height(), cutOffHeight);
		m_landMask = std::make_unique<MapMask>("landMask.png", textureBuffer, Width(), Height(), cutOffHeight);
		m_oceanMask = std::make_unique<MapMask>("oceamMask.png", textureBuffer, Width(), Height(), cutOffHeight, false);
		RegenerateMasks(data);
		
		m_lookupmap = std::make_unique<LookupMap>("lookupTexture.png", Width(), Height());
		RegenerateLookUp(data, progressCallback);

		m_heightmap = std::make_unique<HeightMap>("heightMap1.png", Width(), Height(), m_landMask->GetElevation());
		m_terrainmap = std::make_unique<TerrainMap>("terrainMap.png", m_heightmap->NoiseMap(), Width(), Height(), m_terrainTypes);

	}

	void Map::GenerateMapFromMaskTexture(const std::vector<uint8_t>& textureBuffer, float cutOffHeight,
		const LookupMapData& data, std::function<void(float, std::string_view)> progressCallback)
	{
		if(progressCallback)
		{
			progressCallback(0.0f, "GenerateMapFromHeigthMap");
		}
		
		m_landMask = std::make_unique<MapMask>("landMask.png", textureBuffer, Width(), Height(), cutOffHeight);
		m_oceanMask = std::make_unique<MapMask>("oceamMask.png", textureBuffer, Width(), Height(), cutOffHeight, false);
		RegenerateMasks(textureBuffer);
		
		m_lookupmap = std::make_unique<LookupMap>("lookupTexture.png", Width(), Height());
		RegenerateLookUp(data, progressCallback);
	}

	void Map::RegenerateMasks(const LookupMapData& data)
	{
		if (std::abs(data.cutOffHeight - m_cutOffHeight) > 0.00001)
		{
			m_landMask->RegenerateMask(data.cutOffHeight, true);
			m_oceanMask->RegenerateMask(data.cutOffHeight, false);
		}
	}

	void Map::RegenerateMasks(const std::vector<uint8_t>& maskTextureBuffer)
	{
		if (maskTextureBuffer.size() > 0)
		{
			m_landMask->RegenerateMask(maskTextureBuffer, true);
			m_oceanMask->RegenerateMask(maskTextureBuffer, false);
		}
	}

	void Map::SaveMap(const std::string &filePath) const
	{

	}

	void Map::Reset()
	{
		ClearMapComponent(m_lookupmap.get());
		ClearMapComponent(m_heightmap.get());
		ClearMapComponent(m_terrainmap.get());
		ClearMapComponent(m_landMask.get());
		ClearMapComponent(m_maskmap.get());
		ClearMapComponent(m_oceanMask.get());

		// m_terrainTypes.
		m_cutOffHeight = 0.01f;
	}
	
	void Map::SetSize(unsigned width, unsigned height)
	{
		Reset();
		setDimensions(width, height);
	}
	void Map::SetLookupTileMap(const std::vector<Tile>& tiles)
	{
		m_lookupmap = std::make_unique<LookupMap>("lookupTexture.png", Width(), Height());
		m_lookupmap->SetTileMap(tiles);
	}
	
	std::vector<Tile> Map::GetTiles() const
	{
		if(m_lookupmap == nullptr)
		{
			return std::vector<Tile>();
		}

		return m_lookupmap->GetTiles();
	}

	void Map::SaveMapComponent(MapComponent *component, const std::string &filePath, const char *message) const
	{
		/*if (component)
			component->SaveToFile(filePath);
		else
			std::cout << "ERROR - save " << message << std::endl;*/
	}

	void Map::ClearMapComponent(MapComponent *component, const char *message)
	{
		if (component)
			component->Clear();
		else
			std::cout << "ERROR - clear " << message << std::endl;
	}

	bool Map::IsValid() const
	{
		return m_lookupmap != nullptr && m_lookupmap->GetTiles().size() > 0;
	}
}
