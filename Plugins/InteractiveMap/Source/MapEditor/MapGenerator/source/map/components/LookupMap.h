#pragma once
#include <vector>
#include <cassert>
#include <string>
#include "../MapComponent.h"
#include "../MapSpecs.h"
#include "../../graphics/Rasterizer.h"
#include "../TileMap.h"

namespace MapGenerator
{

	using namespace data;
	// class Texture;
	class MapMask;
	class LookupMap : public MapComponent
	{
		using Diagram = mygal::Diagram<double>;

	public:
		LookupMap(const char *name, unsigned width, unsigned height);

		void RegenerateLookUp(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,  std::function<void(float, std::string_view)> progressCallback = nullptr);
		void RegenerateBorders(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,  std::function<void(float, std::string_view)> progressCallback = nullptr);

		TileMap GenerateTileMapFromMask(const std::shared_ptr<Diagram> &diagram, const NoiseData &borderNoise, float borderThick,
										const MapMask *mask, TileType type,
										std::unordered_set<data::Color> &colorsInUse,
										const char *name = "example.png",
										 std::function<void(float, std::string_view)> progressCallback = nullptr);

		const TileMap& GetTileMap() const;

	private:
		void computeDiagramFromMask(const LookupFeatures &data, MapMask *mask, std::shared_ptr<Diagram> &diagram,  std::function<void(float, std::string_view)> progressCallback = nullptr) const;

		LookupFeatures m_lastLookup;
		std::vector<mygal::Vector2<double>> m_centroids;

		std::unique_ptr<TileMap> m_lookUpTileMap;

		std::shared_ptr<Diagram> m_landDiagram;
		std::shared_ptr<Diagram> m_oceanDiagram;
	};

}