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

		RegenerateBorders(data, landMask, oceanMask, progressCallback);
		if(progressCallback)
		{
			progressCallback(10.0f, "Finished regenerate Lookup");
		}
		
	}

	void LookupMap::RegenerateBorders(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,  std::function<void(float, std::string_view)> progressCallback)
	{
		const auto width = Width();
		const auto height = Height();
		std::unordered_set<data::Color> colorsInUse;
		auto landTileMap = GenerateTileMapFromMask(m_landDiagram, data.borderNoise, data.borderLine, landMask, TileType::LAND, colorsInUse, data.land.searchRadiusBeforeNewTile, progressCallback);
		auto oceanTileMap = GenerateTileMapFromMask(m_oceanDiagram, data.borderNoise, data.borderLine, oceanMask, TileType::WATER, colorsInUse, data.ocean.searchRadiusBeforeNewTile, progressCallback);
		
		TileMap lookUpTileMap = TileMap::BlendTileMap(landTileMap, TileType::LAND, oceanTileMap, TileType::WATER);
		m_lookUpTileMap = std::make_unique<TileMap>(std::move(lookUpTileMap));
		if(progressCallback)
		{
			progressCallback(10.0f, "Finished regenerate Borders");
		}

	}

	TileMap LookupMap::GenerateTileMapFromMask(const std::shared_ptr<Diagram> &diagram, const NoiseData &noiseData, float borderThick,
											   const MapMask *mapMask, TileType type,
											   std::unordered_set<data::Color> &colors,
											   const int searchRadiusBeforeNewTile,
											   std::function<void(float, std::string_view)> progressCallback)
	{
		const auto width = Width();
		const auto height = Height();

		assert(diagram != nullptr);

		TileMap maskTileMap(width, height);

		rasterizer::RasterizeDiagramToTileMap(*diagram, width, height, maskTileMap, noiseData, (double)borderThick);
		if(progressCallback)
		{
			progressCallback(4.0f, "Rasterize Diagram");
		}

		maskTileMap.MarkTilesNotInMaskAsVisited(mapMask->GetMask(), type);
		if(progressCallback)
		{
			progressCallback(4.0f, "Mark Tiles as visited");
		}
		
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
	
		maskTileMap.ColorInBorders(mapMask->GetMask());
		if(progressCallback)
        {
        	progressCallback(4.0f, "Color in borders");
        }
		return maskTileMap;
	}

	const TileMap& LookupMap::GetTileMap() const
	{
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
		auto pointsContr = geomt::generatePointsConstrained<double>(data.tiles, data.seed, true, mask->GetMask());
		diagram = std::make_shared<Diagram>(geomt::generateDiagram(pointsContr));
		geomt::lloydRelaxation(*diagram.get(), data.lloyd, mask->GetMask());
		if(ProgressCallback)
		{
			// ProgressCallback(20.0f, "Generated Lloyd Diagram from Mask");
		}

	}

}