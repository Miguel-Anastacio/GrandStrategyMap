#pragma once
#include "Rasterizer.h"
namespace MapGenerator
{
	namespace rasterizer
	{
		void plotTile(int x, int y, unsigned width, unsigned height, std::vector<Tile> &tileMap)
		{
			if (x >= 0 && x < (int)width && y >= 0 && y < (int)height)
			{
				int index = y * width + x;
				tileMap[index].visited = true;	// Mark the tile as visited
				tileMap[index].isBorder = true; // Mark the tile as visited
			}
		}

	} // namespace MapGeneratorTool
} // namespace rasterizer
