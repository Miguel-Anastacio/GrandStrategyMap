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

        bool fill(int x, int y, std::vector<Tile> &tileMap, const data::Color &newColor, unsigned width, unsigned height);

        mygal::Vector2<int> fillGetCentroidOfPoints(int x, int y, std::vector<Tile> &tileMap,
                                                    const data::Color &newColor, unsigned width, unsigned height);

        void markCentroids(int centrooidX, int centrooidY, std::vector<Tile> &tileMap, const data::Color &newColor, unsigned width, unsigned height);

        void floodFill(std::vector<Tile> &tileMap, const std::vector<mygal::Vector2<double>> &centroids,
                       std::unordered_set<data::Color> &colorsInUse,
                       int width, int height);

    } // namespace algo
} // namespace MapGeneratorTool