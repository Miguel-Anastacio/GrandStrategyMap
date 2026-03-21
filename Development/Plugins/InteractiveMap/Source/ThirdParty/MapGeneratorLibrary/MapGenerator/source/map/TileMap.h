#pragma once
#include "../data/Utils.h"
#include "../data/Color.h"
#include "../../ThirdParty/MyGAL/Vector2.h"
#include <vector>
#include <unordered_set>
#include "../utils/VectorWrapper.h"
#include <unordered_map>

namespace MapGenerator
{
	enum class TileType : uint8_t
	{
		UNDEFINED = 0,
		LAND = 1,
		WATER = 2
	};

	struct Tile
	{
		data::Color color;
		bool visited;
		TileType type;
		bool isBorder;
		mygal::Vector2<int> centroid;

		void MarkCentroid(const mygal::Vector2<int> &point)
		{
			if(this->centroid.x == -1 && this->centroid.y == -1)
			{
				this->centroid = point;
			}
		}

		Tile(bool state, const data::Color &col, TileType typ, const mygal::Vector2<int> &centroid) : color(col), visited(state), type(typ), isBorder(false) {}
		Tile() : color(data::Color(0, 0, 0, 0)), visited(false), type(TileType::UNDEFINED), isBorder(false), centroid(-1, -1) {}
	};

	class Mask;
	class TileMap : public data::Dimensions
	{

	public:
		TileMap(unsigned width, unsigned height);
		TileMap(unsigned width, unsigned height, const std::vector<Tile>& tiles);
		TileMap(const TileMap &) = delete;
		TileMap &operator=(const TileMap &) = delete;

		/**
		 * \brief Default move constructor
		 */
		TileMap(TileMap &&) = default;

		/**
		 * \brief Default move assignment operator
		 */
		TileMap &operator=(TileMap &&) = default;

		void MarkTilesNotInMaskAsVisited(const Mask &mask, TileType type);

		// void SetCells(const std::vector<mygal::Vector2<double>>& centroids);

		void FloodFillTileMap(const std::vector<mygal::Vector2<double>> &centroids,
							  std::unordered_set<data::Color> &colorsUnavailable);

		void FloodFillMissingTiles(int radius = 100);

		inline Tile GetTile(unsigned x, unsigned y) const
		{
			if (x >= Width() || y >= Height())
			{
				return Tile();
			}

			return m_tiles[y * Width() + x];
		}

		std::vector<uint8_t> ConvertTileMapToBuffer() const;
		inline std::vector<Tile> &GetTilesRef()
		{
			return m_tiles;
		}

		inline std::vector<Tile> GetTiles() const
		{
			return m_tiles;
		}

		void ComputeCells();
		void ComputeColorsInUse();

		size_t GetColorsInUse() const;
		const std::unordered_map<mygal::Vector2<int>, data::Color> &GetCellMap() const
		{
			return m_cells;
		}

		const std::unordered_set<data::Color> &GetColors() const
		{
			return m_colors;
		}
		
		void SetColors(const std::unordered_set<data::Color>& colors)
		{
			m_colors = colors;
		}

		bool IsTileOfType(TileType type, unsigned x, unsigned y) const
		{
			if (x > Width() || y > Height())
			{
				return false;
			}
			return m_tiles[y * Width() + x].type == type;
		}

		static TileMap BlendTileMap(const TileMap &tileMap1, TileType type1, const TileMap &tileMap2, TileType type2);

		bool FindClosestTileOfSameType(int x, int y, int radius, Tile &out_color) const;

		void ColorInBorders(const Mask &mask);
		void ColorInBordersByType(const TileType type);
		
		std::vector<uint8_t> GetOceanTileMap() const;
		std::vector<uint8_t> GetLandTileMap() const;
		std::vector<uint8_t> GetBordersTileMap(const data::Color& borderColor = data::Color(0, 0, 0, 255), const data::Color& notBorderColor = data::Color(255, 255, 255, 255)) const;
		std::vector<uint8_t> GetVisitedTileMap(const data::Color& visitedColor = data::Color(255, 0, 0, 255), const data::Color& notVisitedColor = data::Color(255, 255, 255, 255)) const;
		std::vector<uint8_t> GetCentroidTileMap() const;
		std::vector<uint8_t> GetUndefinedTileMap(const data::Color& defined = data::Color(0, 0, 0, 255), const data::Color& undefined = data::Color(255, 0, 0, 255)) const;
		std::vector<uint8_t> GetMapOfPointsWhereColorDifferentThanCentroid(const data::Color& different = data::Color(255, 0, 0, 255), const data::Color& same = data::Color(0, 0, 0, 255)) const;
		std::vector<mygal::Vector2<int>> GetPointsWhereTileDoesNotMatchCentroid() const;
		

		void SetTiles(const std::vector<Tile>& tiles)
		{
			m_tiles = tiles;
			ComputeCells();
			ComputeColorsInUse();
		}

		void MarkBorderOnTileMap(const std::vector<uint8_t>& borderBuffer, const TileType type, const data::Color& borderColor = data::Color(0, 0, 255, 255));
	
		template <typename Func>
		void forEachTile(Func&& func)
		{
			for (unsigned i = 0; i < Height(); i++)
			{
				for (unsigned j = 0; j < Width(); j++)
				{
					const int tileIndex = (i * Width() + j);
					const int index = tileIndex * 4;
					func(m_tiles[tileIndex], index);
				}
			}
		}
		
	private:
		template <typename Func>
		void forEachTile(Func&& func) const
		{
			for (unsigned i = 0; i < Height(); i++)
			{
				for (unsigned j = 0; j < Width(); j++)
				{
					const int tileIndex = (i * Width() + j);
					const int index = tileIndex * 4;
					func(m_tiles[tileIndex], index);
				}
			}
		}
		
		std::vector<Tile> m_tiles;
		std::unordered_map<mygal::Vector2<int>, data::Color> m_cells;
		
		std::unordered_set<data::Color> m_colors;
		
	
		std::vector<uint8_t> GetTileMapOfType(TileType type) const; 
		void PrintTileMapColors();

		bool isValid(int x, int y)
		{
			if (x >= (int)Width() || y >= (int)Height() || x < 0 || y < 0)
			{
				return false;
			}
			return true;
		}

		static void fillBufferPosWithColor(int index, std::vector<uint8_t>& buffer, const data::Color& color);
		

		template <typename Func>
		std::vector<uint8_t> GetTileMap(Func&& predicate) const
		{
			std::vector<uint8_t> buffer(m_tiles.size() * 4);
			forEachTile([&](const Tile& tile, unsigned bufferIndex)
			{
				if (predicate(tile))
				{
					fillBufferPosWithColor(bufferIndex, buffer, tile.color);
				}
				else
				{
					fillBufferPosWithColor(bufferIndex, buffer, data::Color(0, 0, 0, 0));
				}
			});
			return buffer;
		}

		template <typename Func>
		std::vector<uint8_t> GetTileMap(Func&& predicate, const data::Color& colorOnFullfil, const data::Color& colorOnNotFullfil) const
		{
			std::vector<uint8_t> buffer(m_tiles.size() * 4);
			forEachTile([&](const Tile& tile, unsigned bufferIndex)
			{
				if (predicate(tile))
				{
					fillBufferPosWithColor(bufferIndex, buffer, colorOnFullfil);
				}
				else
				{
					fillBufferPosWithColor(bufferIndex, buffer, colorOnNotFullfil);
				}
			});
			return buffer;
		}

	};

	std::ostream &operator<<(std::ostream &os, const TileMap &map);



} // namespace MapGeneratorTool
