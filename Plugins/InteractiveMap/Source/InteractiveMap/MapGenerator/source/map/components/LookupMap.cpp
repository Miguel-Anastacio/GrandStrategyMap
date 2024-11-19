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

	void LookupMap::RegenerateLookUp(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask)
	{
		const auto width = Width();
		const auto height = Height();

		computeDiagramFromMask(data.land, landMask, m_landDiagram);
		computeDiagramFromMask(data.ocean, oceanMask, m_oceanDiagram);

		RegenerateBorders(data, landMask, oceanMask);

	}

	void LookupMap::RegenerateBorders(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask)
	{
		const auto width = Width();
		const auto height = Height();
		std::unordered_set<data::Color> colorsInUse;
		auto landTileMap = GenerateTileMapFromMask(m_landDiagram, data.borderNoise, data.borderLine, landMask, TileType::LAND, colorsInUse, "landMaskLookUp.png");
		auto oceanTileMap = GenerateTileMapFromMask(m_oceanDiagram, data.borderNoise, data.borderLine, oceanMask, TileType::WATER, colorsInUse, "oceanMaskLookUp.png");


		TileMap lookUpTileMap = TileMap::BlendTileMap(landTileMap, TileType::LAND, oceanTileMap, TileType::WATER);
		m_lookUpTileMap = std::make_unique<TileMap>(std::move(lookUpTileMap));

	}

	TileMap LookupMap::GenerateTileMapFromMask(const std::shared_ptr<Diagram> &diagram, const NoiseData &noiseData, float borderThick,
											   const MapMask *mapMask, TileType type,
											   std::unordered_set<data::Color> &colors,
											   const char *name)
	{
		const auto width = Width();
		const auto height = Height();

		assert(diagram != nullptr);

		TileMap maskTileMap(width, height);

		rasterizer::RasterizeDiagramToTileMap(*diagram, width, height, maskTileMap, noiseData, (double)borderThick);

		maskTileMap.MarkTilesNotInMaskAsVisited(mapMask->GetMask(), type);

		std::vector<mygal::Vector2<double>> centroids = geomt::GetCentroidsOfDiagram(*diagram);
		maskTileMap.FloodFillTileMap(centroids, colors);

		maskTileMap.FloodFillMissingTiles(100);
	
		maskTileMap.ColorInBorders(mapMask->GetMask());

		return maskTileMap;
	}

	const TileMap& LookupMap::GetTileMap() const
	{
		return *m_lookUpTileMap;
	}

	void LookupMap::computeDiagramFromMask(const LookupFeatures &data, MapMask *mask, std::shared_ptr<Diagram> &diagram) const
	{
		const auto width = Width();
		const auto height = Height();

		auto pointsContr = geomt::generatePointsConstrained<double>(data.tiles, data.seed, true, mask->GetMask());
		diagram = std::make_shared<Diagram>(geomt::generateDiagram(pointsContr));
		geomt::lloydRelaxation(*diagram.get(), data.lloyd, mask->GetMask());

	}

}