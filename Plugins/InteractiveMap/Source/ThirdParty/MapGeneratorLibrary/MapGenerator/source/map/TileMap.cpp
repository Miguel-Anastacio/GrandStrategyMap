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

	TileMap::TileMap(unsigned width, unsigned height, const std::vector<Tile>& tiles)
		: Dimensions(width, height)
		, m_tiles(std::move(tiles))
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
		const auto width = Width();
		const auto height = Height();
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
			buffer.emplace_back(tile.color.B);
			buffer.emplace_back(tile.color.G);
			buffer.emplace_back(tile.color.R);
			buffer.emplace_back(tile.color.A);
		}

		return buffer;
	}

	uint8_t* TileMap::ConvertTileMapToRawBuffer() const
	{
		uint8_t* buffer = new uint8_t[m_tiles.size() * 4];
		
		for (unsigned i = 0; i < Height(); i++)
		{
			for (unsigned j = 0; j < Width(); j++)
			{
				const int tileIndex = (i * Width() + j);
				const int index = tileIndex * 4;
				buffer[index] = m_tiles[tileIndex].color.B;
				buffer[index + 1] = m_tiles[tileIndex].color.G;
				buffer[index + 2] = m_tiles[tileIndex].color.R;
				buffer[index + 3] = m_tiles[tileIndex].color.A;
				
			}
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

		const auto height = tileMap1.Height();
		const auto width = tileMap1.Width();

		TileMap newTileMap(width, height);
		auto &newTiles = newTileMap.GetTilesRef();

		const auto tiles1 = tileMap1.GetTiles();
		const auto tiles2 = tileMap2.GetTiles();

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
		if (radius == 0)
			return false;
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

	uint8_t* TileMap::GetLandTileMap() const
	{
		return GetTileMapOfType(TileType::LAND);
	}

	uint8_t* TileMap::GetOceanTileMap() const
	{
		return GetTileMapOfType(TileType::WATER);
	}
	
	uint8_t* TileMap::GetBordersTileMap(const data::Color& borderColor, const data::Color& notBorderColor) const
	{
		return GetTileMap([](const Tile &tile)
			{
				return tile.isBorder;
			}, borderColor, notBorderColor);
	}
	
	uint8_t* TileMap::GetVisitedTileMap(const data::Color& visitedColor, const data::Color& notVisitedColor) const
	{
		return GetTileMap([](const Tile &tile)
			{
				return !tile.visited;
			}, visitedColor, notVisitedColor);
	}

	uint8_t* TileMap::GetCentroidTileMap() const
	{
		uint8_t* buffer = new uint8_t[m_tiles.size() * 4];
		forEachTile([&](const Tile& tile, unsigned bufferIndex)
		{
			auto centroidIndex = tile.centroid.y * Width() + tile.centroid.x;
			auto index = bufferIndex / 4;
			
			if(centroidIndex < m_tiles.size() && index == centroidIndex)
			{
				data::Color color(255, 0, 0, 255);
				fillBufferPosWithColor(bufferIndex, buffer, color);
			}
			else
			{
				data::Color color(0, 0, 0, 255);
				fillBufferPosWithColor(bufferIndex, buffer, color);
			}
		});
		return buffer;
	}

	uint8_t* TileMap::GetTileMapOfType(TileType type) const
	{
		return GetTileMap([type](const Tile &tile)
			{
				return tile.type == type;
			});
	}
	
	uint8_t* TileMap::GetUndefinedTileMap(const data::Color& defined, const data::Color& undefined) const
	{
		return GetTileMap([](const Tile &tile)
			{
				return tile.type == TileType::UNDEFINED;
			}, defined, undefined);
	}

	uint8_t* TileMap::GetTileMap(std::function<bool(const Tile&)> predicate) const
	{
		uint8_t* buffer = new uint8_t[m_tiles.size() * 4];
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

	uint8_t* TileMap::GetTileMap(std::function<bool(const Tile&)> predicate, const data::Color& colorOnFullfil, const data::Color& colorOnNotFullfil) const
	{
		uint8_t* buffer = new uint8_t[m_tiles.size() * 4];
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

	void TileMap::MarkBorderOnTileMap(const std::vector<uint8_t>& borderBuffer, const data::Color& borderColor)
	{
		if(borderBuffer.size() != m_tiles.size() * 4)
		{
			std::cout << "border does not match tile size\n";
			return;
		}

		for (unsigned i = 0; i < Height(); i++)
		{
			for (unsigned j = 0; j < Width(); j++)
			{
				const int tileIndex = (i * Width() + j);
				const int index = tileIndex * 4;
				const uint8_t r = borderBuffer[index];
				const uint8_t g = borderBuffer[index + 1];
				const uint8_t b = borderBuffer[index + 2];
				const uint8_t a = borderBuffer[index + 3];
				const data::Color &color = data::Color(r, g, b, a);
				if(color != data::Color(255, 255, 255, 255))
				{
					m_tiles[tileIndex].visited = true;
					m_tiles[tileIndex].isBorder = true;
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

	void TileMap::fillBufferPosWithColor(int index, uint8_t* buffer, const data::Color& color)
	{
		buffer[index]	  = color.B;
		buffer[index + 1] = color.G;
		buffer[index + 2] = color.R;
		buffer[index + 3] = color.A;
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