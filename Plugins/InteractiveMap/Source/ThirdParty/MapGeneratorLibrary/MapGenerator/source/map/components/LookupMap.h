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
	enum class TileMapGenType
	{
		VoronoiDiagram,
		UserUploadedBorderForLand,
		UserUploadedBorderForOcean,
	};

	using namespace data;
	// class Texture;
	class MapMask;
	class LookupMap : public MapComponent
	{
		using Diagram = mygal::Diagram<double>;

	public:
		LookupMap(const char *name, unsigned width, unsigned height, ALogger::Logger& logger);

		void RegenerateLookUp(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,  std::function<void(float, std::string_view)> progressCallback = nullptr);
		void RegenerateLookUp(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,
								const TileMapGenType tileMapGenType,
								std::function<void(float, std::string_view)> progressCallback = nullptr);
		void RegenerateBorders(const LookupMapData &data, MapMask *landMask, MapMask *oceanMask,  const TileMapGenType genType = TileMapGenType::VoronoiDiagram,
								std::function<void(float, std::string_view)> progressCallback = nullptr);

		// void RasterizeBorders()
		TileMap GenerateLandTileMapFromMask(const LookupMapData &data, std::unordered_set<data::Color> &colorsInUse,
											const MapMask* landMask,
											const TileMapGenType genType = TileMapGenType::VoronoiDiagram,
											std::function<void(float, std::string_view)> progressCallback = nullptr) const;
		
		TileMap GenerateOceanTileMapFromMask(const LookupMapData &data, std::unordered_set<data::Color> &colorsInUse,
											 const MapMask* oceanMask,
											 const TileMapGenType genType = TileMapGenType::VoronoiDiagram,	
											 std::function<void(float, std::string_view)> progressCallback = nullptr) const;
		

		const TileMap& GetTileMap() const;
		std::vector<Tile> GetTiles() const;
		void SetTileMap(const std::vector<Tile>& tiles);
		void SetBorderBufferRef(const std::shared_ptr<std::vector<uint8_t>>& borderDataBuffer);

	private:
		TileMap GenerateTileMapFromMask(const std::shared_ptr<Diagram> &diagram, const NoiseData &borderNoise, float borderThick,
										const MapMask *mask, TileType type,
										std::unordered_set<data::Color> &colorsInUse,
										const int searchRadiusBeforeNewTile = 100,
										 std::function<void(float, std::string_view)> progressCallback = nullptr) const;
		TileMap GenerateTileMapFromMask(const MapMask *mask, TileType type, std::unordered_set<data::Color> &colorsInUse,
										const std::vector<uint8_t>& borderDataBuffer, const int searchRadiusBeforeNewTile = 50,
										std::function<void(float, std::string_view)> progressCallback = nullptr) const;
		
		void computeDiagramFromMask(const LookupFeatures &data, MapMask *mask, std::shared_ptr<Diagram> &diagram,  std::function<void(float, std::string_view)> progressCallback = nullptr) const;

		std::shared_ptr<std::vector<uint8_t>> getBorderDataBuffer() const
		{
			auto borderBufferShared = m_borderBufferRef.lock();
			if (!borderBufferShared)
			{
				// Handle null / expired pointer — weak_ptr has expired
				// You could throw, return an error, or use a fallback
				throw std::runtime_error("Border buffer reference is no longer valid.");
			}
			return borderBufferShared;
		}
		LookupFeatures m_lastLookup;
		std::vector<mygal::Vector2<double>> m_centroids;

		std::unique_ptr<TileMap> m_lookUpTileMap;

		std::shared_ptr<Diagram> m_landDiagram;
		std::shared_ptr<Diagram> m_oceanDiagram;
		std::weak_ptr<std::vector<uint8_t>> m_borderBufferRef;
	};

}