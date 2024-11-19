#include "TileMap.h"
#include <cmath>
#include "../utils/Algo.h"
#include "../data/Mask.h"
#include "../data/Utils.h"
//#include "Utilities/Timer.h"
//#include "Utilities/Logger.h"
namespace MapGenerator
{
	TileMap::TileMap(unsigned width, unsigned height) : Dimensions(width, height), m_tiles(width * height, Tile())
	{
	}
	void TileMap::MarkTilesNotInMaskAsVisited(const Mask &mask, TileType type)
	{
		auto width = Width();
		auto height = Height();
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				if (!mask.isInMask(x, y))
				{
					m_tiles[y * width + x].visited = true;
				}
				else
				{
					m_tiles[y * width + x].type = type;
				}
			}
		}
	}

	void TileMap::FloodFillTileMap(const std::vector<mygal::Vector2<double>> &centroids, std::unordered_set<data::Color> &colorsUnavailable)
	{
		auto width = Width();
		auto height = Height();
		algo::floodFill(m_tiles, centroids, colorsUnavailable, width, height);
		m_colors = colorsUnavailable;
	}

	void TileMap::FloodFillMissingTiles(int radius)
	{
		auto width = Width();
		auto height = Height();
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				auto index = y * width + x;
				if (!m_tiles[index].visited)
				{
					data::Color color;
					if (FindColorOfClosestTileOfSameType(x, y, radius, color))
					{
						algo::fill(x, y, m_tiles, color, width, height);
					}
					else
					{
						color = data::GetRandomColorNotInSet(m_colors);

						mygal::Vector2<int> point = algo::fillGetCentroidOfPoints(x, y, m_tiles, color, width, height);
					}
				}
			}
		}
	}
	std::vector<uint8_t> TileMap::ConvertTileMapToBuffer() const
	{
		std::vector<uint8_t> buffer;
		buffer.reserve(m_tiles.size() * 4);
		for (auto &tile : m_tiles)
		{
			buffer.emplace_back(tile.color.R);
			buffer.emplace_back(tile.color.G);
			buffer.emplace_back(tile.color.B);
			buffer.emplace_back(tile.color.A);
		}

		return buffer;
	}

	void TileMap::ComputeCells()
	{
		auto height = Height();
		auto width = Width();
		m_cells.clear();

		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				const int index = y * width + x;
				mygal::Vector2<int> centroid = m_tiles[index].centroid;
				if (isValid(centroid.x, centroid.y))
				{
					if (!m_cells.contains(m_tiles[index].centroid))
					{
						m_cells.emplace(centroid, GetTile(centroid.x, centroid.y).color);
					}
				}
			}
		}
	}
	void TileMap::ComputeColorsInUse()
	{
		auto height = Height();
		auto width = Width();
		m_colors.clear();
		m_colors.reserve(m_cells.size());

		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				const unsigned index = y * width + x;
				m_colors.emplace(m_tiles[index].color);
			}
		}
	}

	size_t TileMap::GetColorsInUse() const
	{
		return m_colors.size();
	}

	TileMap TileMap::BlendTileMap(const TileMap &tileMap1, TileType type1, const TileMap &tileMap2, TileType type2)
	{

		auto height = tileMap1.Height();
		auto width = tileMap1.Width();

		TileMap newTileMap(width, height);
		auto &newTiles = newTileMap.GetTilesRef();

		auto tiles1 = tileMap1.GetTiles();
		auto tiles2 = tileMap2.GetTiles();

		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				const unsigned index = y * width + x;
				if (tiles1[index].type == type1)
				{
					newTiles[index] = tiles1[index];
				}
				else if (tiles2[index].type == type2)
				{
					newTiles[index] = tiles2[index];
				}
			}
		}

		newTileMap.ComputeCells();
		newTileMap.ComputeColorsInUse();
		newTileMap.PrintTileMapColors();

		return newTileMap;
	}

	bool TileMap::FindColorOfClosestTileOfSameType(int x, int y, int radius, data::Color &out_color) const
	{
		auto width = Width();
		auto height = Height();
		std::unordered_set<mygal::Vector2<int>> tilesVisited;
		const int startIdx = y * Width() + x;
		auto targetType = m_tiles[startIdx].type;
		std::queue<std::pair<int, int>> queue;

		// Initialize BFS queue with the starting position
		queue.push({x, y});

		while (!queue.empty())
		{
			auto [cx, cy] = queue.front();
			queue.pop();
			if (cx < 0 || cx >= (int)width || cy < 0 || cy >= (int)height)
				continue;
			if (tilesVisited.contains(mygal::Vector2<int>(cx, cy)))
				continue;

			const Tile &tile = m_tiles[cy * width + cx];

			if (tile.type == targetType && (tile.visited) && (!tile.isBorder))
			{
				out_color = tile.color;
				return true;
			}
			else
			{
				tilesVisited.insert(mygal::Vector2<int>(cx, cy));
			}

			// did not find tile in radius
			if (std::abs(x + radius) < std::abs(cx) || std::abs(y + radius) < std::abs(cy))
			{
				return false;
			}

			queue.push({cx + 1, cy});
			queue.push({cx - 1, cy});
			queue.push({cx, cy + 1});
			queue.push({cx, cy - 1});
		}

		return false;
	}
	void TileMap::ColorInBorders(const Mask &mask)
	{
		auto width = Width();
		auto height = Height();
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				auto index = y * width + x;
				if (m_tiles[index].isBorder && mask.isInMask(x, y))
				{
					data::Color color;
					if (FindColorOfClosestTileOfSameType(x, y, width, color))
					{
						m_tiles[index].color = color;
					}
				}
			}
		}
	}

	void TileMap::PrintTileMapColors()
	{
		/*int index = 0;

		std::cout << "\n Tile Map Colors" << "\n";
		for (auto& col : m_colors)
		{
			std::cout << "Index " << index << "\n";
			std::cout << "Color - " << col << "\n";
			index++;
		}	*/
	}

	std::ostream &operator<<(std::ostream &os, const TileMap &map)
	{
		int index = 0;
		os << "\n";
		os << "Tile Map Cells" << "\n";
		for (const auto &cell : map.GetCellMap())
		{
			os << "Cell Index " << index << "\n";
			os << "Centroid: " << cell.first << "\n";
			os << "Color - " << cell.second << "\n\n";
			index++;
		}

		index = 0;
		os << "Tile Map Colors" << "\n";
		for (const auto &col : map.GetColors())
		{
			os << "Index " << index << "\n";
			os << "Color- " << col << "\n\n";
			index++;
		}

		return os;
	}
} // namespace MapGeneratorTool