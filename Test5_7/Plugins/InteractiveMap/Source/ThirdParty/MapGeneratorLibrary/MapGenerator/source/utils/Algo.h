#pragma once
#include <vector>
#include "../map/TileMap.h"
#include "../data/Color.h"
#include "VectorWrapper.h"
#include <stack>
namespace MapGenerator
{
    namespace algo
    {
        bool fill(int x, int y, std::vector<Tile> &tileMap, const data::Color &newColor, unsigned width, unsigned height, bool markCentroid =false);

        void fillGetCentroidOfPoints(int x, int y, std::vector<Tile> &tileMap,
                                                    const data::Color &newColor, unsigned width, unsigned height);

        void floodFill(std::vector<Tile> &tileMap, const std::vector<mygal::Vector2<double>> &centroids,
                       std::unordered_set<data::Color> &colorsInUse,
                       int width, int height);

        bool FindClosestTileOfType(int x, int y, int radius, const std::vector<Tile> &tileMap, unsigned width, unsigned height, const TileType targetType, Tile &out_tile);

    } // namespace algo
} // namespace MapGeneratorTool