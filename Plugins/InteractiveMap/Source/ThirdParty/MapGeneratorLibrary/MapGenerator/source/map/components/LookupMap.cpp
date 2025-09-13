#pragma once
#include "LookupMap.h" 
#include <algorithm>
#include "../../../thirdParty/MyGAL/Vector2.h"
#include "MapMask.h"
#include "../../utils/ComputeGeometry.h"
#include "../../data/Mask.h"
#include <queue>
#include "../../utils/Algo.h"
#include "../../data/Color.h"
#include "../../utils/VectorWrapper.h"
namespace MapGenerator
{
	LookupMap::LookupMap(const char *name, unsigned width, unsigned height)
		: MapComponent(width, height, name)
	{
	}

	void LookupMap::RegenerateLookUp(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,  std::function<void(float, std::string_view)> progressCallback)
	{
		const auto width = Width();
		const auto height = Height();

		computeDiagramFromMask(data.land, landMask, m_landDiagram, progressCallback);
		computeDiagramFromMask(data.ocean, oceanMask, m_oceanDiagram, progressCallback);

		RegenerateBorders(data, landMask, oceanMask, TileMapGenType::VoronoiDiagram, progressCallback);
		if(progressCallback)
		{
			progressCallback(10.0f, "Finished regenerate Lookup");
		}
		
	}

	void LookupMap::RegenerateLookUp(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,
										 const TileMapGenType tileMapGenType,
										 std::function<void(float, std::string_view)> progressCallback)
	{
		const auto width = Width();
		const auto height = Height();

		switch (tileMapGenType)
		{
		case TileMapGenType::UserUploadedBorderForLand:
			computeDiagramFromMask(data.ocean, oceanMask, m_oceanDiagram, progressCallback);
			break;
		case TileMapGenType::UserUploadedBorderForOcean:
			computeDiagramFromMask(data.land, landMask, m_landDiagram, progressCallback);
			break;
		default:
			computeDiagramFromMask(data.land, landMask, m_landDiagram, progressCallback);
			computeDiagramFromMask(data.ocean, oceanMask, m_oceanDiagram, progressCallback);
		}
		
		RegenerateBorders(data, landMask, oceanMask, tileMapGenType, progressCallback);
		if(progressCallback)
		{
			progressCallback(10.0f, "Finished regenerate Lookup");
		}
	}


	void LookupMap::RegenerateBorders(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask, const TileMapGenType genType, std::function<void(float, std::string_view)> progressCallback)
	{
		std::unordered_set<data::Color> colorsInUse;
		const auto landTileMap = GenerateLandTileMapFromMask(data, colorsInUse, landMask, genType, progressCallback);
		const auto oceanTileMap = GenerateOceanTileMapFromMask(data, colorsInUse, oceanMask, genType, progressCallback);
		
		TileMap lookUpTileMap = TileMap::BlendTileMap(landTileMap, TileType::LAND, oceanTileMap, TileType::WATER);
		m_lookUpTileMap = std::make_unique<TileMap>(std::move(lookUpTileMap));

	}

	TileMap LookupMap::GenerateLandTileMapFromMask(const LookupMapData &data, std::unordered_set<data::Color> &colorsInUse,
													const MapMask* landMask, const TileMapGenType genType,
													std::function<void(float, std::string_view)> progressCallback) const
	{
		switch (genType)
		{	
		case TileMapGenType::UserUploadedBorderForLand:
			return GenerateTileMapFromMask(landMask, TileType::LAND, colorsInUse, *getBorderDataBuffer(), data.land.searchRadiusBeforeNewTile, progressCallback);
			break;
		default:
			return GenerateTileMapFromMask(m_landDiagram, data.borderNoise, data.borderLine, landMask, TileType::LAND, colorsInUse, data.land.searchRadiusBeforeNewTile, progressCallback);	
		}
	}

	TileMap LookupMap::GenerateOceanTileMapFromMask(const LookupMapData &data, std::unordered_set<data::Color> &colorsInUse,
													const MapMask* oceanMask,const TileMapGenType genType,
													std::function<void(float, std::string_view)> progressCallback) const
	{
		switch (genType)
		{
		case TileMapGenType::UserUploadedBorderForOcean:
			return GenerateTileMapFromMask(oceanMask, TileType::WATER, colorsInUse, *getBorderDataBuffer(), data.ocean.searchRadiusBeforeNewTile, progressCallback);
		default:
			return GenerateTileMapFromMask(m_oceanDiagram, data.borderNoise, data.borderLine, oceanMask, TileType::WATER, colorsInUse, data.ocean.searchRadiusBeforeNewTile, progressCallback);
		}
	}

	TileMap LookupMap::GenerateTileMapFromMask(const std::shared_ptr<Diagram> &diagram, const NoiseData &noiseData, float borderThick,
											   const MapMask *mapMask, TileType type,
											   std::unordered_set<data::Color> &colors,
											   const int searchRadiusBeforeNewTile,
											   std::function<void(float, std::string_view)> progressCallback) const
	{

		const auto width = Width();
		const auto height = Height();
		
		TileMap maskTileMap(width, height);
		// when map gen is done with one tile then the diagram is null
		if(diagram != nullptr)
		{
			// Border mark approach
			rasterizer::RasterizeDiagramToTileMap(*diagram, width, height, maskTileMap, noiseData, (double)borderThick);
			///////////////////////////////////////////////////////////////
			if(progressCallback)
			{
				progressCallback(4.0f, "Rasterize Diagram");
			}

			maskTileMap.MarkTilesNotInMaskAsVisited(mapMask->GetMask(), type);
			if(progressCallback)
			{
				progressCallback(4.0f, "Mark Tiles as visited");
			}

			// Flood fill approach
			std::vector<mygal::Vector2<double>> centroids = geomt::GetCentroidsOfDiagram(*diagram);
			maskTileMap.FloodFillTileMap(centroids, colors);
			if(progressCallback)
			{
				progressCallback(4.0f, "Flood Fill");
			}

			maskTileMap.FloodFillMissingTiles(searchRadiusBeforeNewTile);
			if(progressCallback)
			{
				progressCallback(4.0f, "FloodFill MissingTiles");
			}
			////////////////////////////////////////////////////////////////////////
			maskTileMap.ColorInBorders(mapMask->GetMask());
			if(progressCallback)
			{
				progressCallback(4.0f, "Color in borders");
			}
		}
		else
		{
			maskTileMap.MarkTilesNotInMaskAsVisited(mapMask->GetMask(), type);
			if(progressCallback)
			{
				progressCallback(4.0f, "Mark Tiles as visited");
			}
			
			maskTileMap.FloodFillMissingTiles(Width() * Height());
			if(progressCallback)
			{
				progressCallback(4.0f, "FloodFill MissingTiles");
			}
		}
		return maskTileMap;
	}

	TileMap LookupMap::GenerateTileMapFromMask(const MapMask* mapMask, TileType type, std::unordered_set<data::Color> &colorsInUse,
								const std::vector<uint8_t>& borderDataBuffer, const int searchRadiusBeforeNewTile,
								std::function<void(float, std::string_view)> progressCallback) const
	{
		const auto width = Width();
		const auto height = Height();

		TileMap maskTileMap(width, height);
		maskTileMap.SetColors(colorsInUse);
		maskTileMap.MarkBorderOnTileMap(borderDataBuffer);
		if(progressCallback)
		{
			progressCallback(4.0f, "Mark Borders");
		}
		
		maskTileMap.MarkTilesNotInMaskAsVisited(mapMask->GetMask(), type);
		if(progressCallback)
		{
			progressCallback(4.0f, "Mark Tiles as visited");
		}
		
		maskTileMap.FloodFillMissingTiles(searchRadiusBeforeNewTile);
		if(progressCallback)
		{
			progressCallback(4.0f, "FloodFill MissingTiles");
		}
		
		maskTileMap.ColorInBorders(mapMask->GetMask());
		if(progressCallback)
		{
			progressCallback(4.0f, "Color in borders");
		}
		return maskTileMap;
	}

	const TileMap& LookupMap::GetTileMap() const
	{
		assert(m_lookUpTileMap != nullptr);
		return *m_lookUpTileMap;
	}

	void LookupMap::computeDiagramFromMask(const LookupFeatures &data, MapMask *mask, std::shared_ptr<Diagram> &diagram,
	 std::function<void(float, std::string_view)> ProgressCallback) const
	{
		const auto width = Width();
		const auto height = Height();
		if(ProgressCallback)
		{
			ProgressCallback(2.0f, "Before Point Generation");
		}
		if(data.tiles > 1)
		{
			auto pointsContr = geomt::generatePointsConstrained<double>(data.tiles, data.seed, true, mask->GetMask());
			diagram = std::make_shared<Diagram>(geomt::generateDiagram(pointsContr));
			geomt::lloydRelaxation(*diagram.get(), data.lloyd, mask->GetMask());
		}
		else
		{
			diagram = nullptr;
		}
		
		if(ProgressCallback)
		{
			ProgressCallback(20.0f, "Generated Lloyd Diagram from Mask");
		}

	}

	void LookupMap::SetTileMap(const std::vector<Tile>& tiles)
	{
		m_lookUpTileMap = std::make_unique<TileMap>(Width(), Height(), std::move(tiles));
	}

	std::vector<Tile> LookupMap::GetTiles() const
	{
		if(m_lookUpTileMap == nullptr)
			return std::vector<Tile>();
		
		return m_lookUpTileMap->GetTiles();
	}

	void LookupMap::SetBorderBufferRef(const std::shared_ptr<std::vector<uint8_t>>& borderDataBuffer)
	{
		m_borderBufferRef = borderDataBuffer;
	}

}