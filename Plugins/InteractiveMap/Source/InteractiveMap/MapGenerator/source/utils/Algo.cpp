#include "Algo.h"
#include <algorithm>
namespace MapGenerator
{
    namespace algo
    {
        bool fill(int x, int y, std::vector<Tile> &tileMap, const data::Color &newColor, unsigned width, unsigned height)
        {
            const int index = y * width + x;
            const auto tileType = tileMap[index].type;
            auto wid = static_cast<int>(width);
            auto hgt = static_cast<int>(height);

            if (x >= wid || x < 0 || y >= hgt || y < 0 || tileMap[index].color == newColor || tileMap[index].visited)
            {
                return false;
            }

            // Stack for iterative approach
            std::stack<std::pair<int, int>> stack;
            stack.push({x, y});

            while (!stack.empty())
            {
                auto [cx, cy] = stack.top();
                stack.pop();

                // Check boundary conditions again and ensure tile hasn't been visited
                if (cx < 0 || cx >= wid || cy < 0 || cy >= hgt)
                    continue;
                Tile &tile = tileMap[cy * wid + cx];

                if (tile.visited || tile.color == newColor)
                    continue;

                if (tile.type != tileType)
                    continue;

                // Mark tile as visited and set its color
                tile.color = newColor;
                tile.visited = true;

                // Push neighboring tiles onto the stack
                stack.push({cx + 1, cy});
                stack.push({cx - 1, cy});
                stack.push({cx, cy + 1});
                stack.push({cx, cy - 1});
            }

            return true;
        }

        mygal::Vector2<int> fillGetCentroidOfPoints(int x, int y, std::vector<Tile> &tileMap, const data::Color &newColor, unsigned width, unsigned height)
        {
            auto wid = static_cast<int>(width);
            auto hgt = static_cast<int>(height);

            const int index = y * width + x;
            const auto tileType = tileMap[index].type;
            mygal::Vector2<int> pointsSum;
            std::vector<Tile *> tilesRef;
            int count = 0;

            if (x >= wid || x < 0 || y >= hgt || y < 0 || tileMap[index].color == newColor || tileMap[index].visited)
            {
                return mygal::Vector2<int>(-1, -1);
            }
            // Stack for iterative approach
            std::stack<std::pair<int, int>> stack;
            stack.push({x, y});

            while (!stack.empty())
            {
                auto [cx, cy] = stack.top();
                stack.pop();

                // Check boundary conditions again and ensure tile hasn't been visited
                if (cx < 0 || cx >= wid || cy < 0 || cy >= hgt)
                    continue;
                Tile &tile = tileMap[cy * wid + cx];

                if (tile.visited || tile.color == newColor)
                    continue;

                if (tile.type != tileType)
                    continue;

                // Mark tile as visited and set its color
                tile.color = newColor;
                tile.visited = true;
                pointsSum += mygal::Vector2(cx, cy);
                count++;

                // Push neighboring tiles onto the stack
                stack.push({cx + 1, cy});
                stack.push({cx - 1, cy});
                stack.push({cx, cy + 1});
                stack.push({cx, cy - 1});
                tilesRef.emplace_back(&tile);
            }

            const mygal::Vector2<int> centroid = pointsSum / count;
            for (auto &tile : tilesRef)
            {
                tile->centroid = centroid;
            }

            return centroid;
        }

        void markCentroids(int centrooidX, int centrooidY, std::vector<Tile> &tileMap, const data::Color &newColor, unsigned width, unsigned height)
        {
            for (unsigned y = 0; y < height; y++)
            {
                for (unsigned x = 0; x < width; x++)
                {
                    auto index = y * width + x;
                    if (tileMap[index].color == newColor)
                    {
                        tileMap[index].centroid = mygal::Vector2(centrooidX, centrooidY);
                    }
                }
            }
        }

        void floodFill(std::vector<Tile> &tileMap, const std::vector<mygal::Vector2<double>> &centroids,
                       std::unordered_set<data::Color> &colorsInUse,
                       int width, int height)
        {
            std::vector<uint8_t> buffer(tileMap.size() * 4);
            for (auto center : centroids)
            {
                auto x = static_cast<int>(center.x * width);
                auto y = static_cast<int>(center.y * height);
                data::Color color = data::GetRandomColorNotInSet(colorsInUse);

                x = std::clamp(x, 0, width - 1);
                y = std::clamp(y, 0, height - 1);

                if (fill(x, y, tileMap, color, width, height))
                {
                    colorsInUse.insert(color);
                    markCentroids(x, y, tileMap, color, width, height);
                }
            }
        }

    } // namespace algo
} // namespace MapGeneratorTool