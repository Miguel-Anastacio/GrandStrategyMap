#pragma once
#include <vector>
#include "../../ThirdParty/MyGAL/Diagram.h"
#include "../utils/ComputeGeometry.h"
#include "../data/Color.h"
#include "../map/TileMap.h"
#include "../map/MapSpecs.h"
#include "../utils/VectorWrapper.h"
namespace MapGenerator
{
	namespace rasterizer
	{

		void plotTile(int x, int y, unsigned width, unsigned height, std::vector<Tile> &tileMap);

		template <typename T>
		static void Line(const mygal::Vector2<T> &start, const mygal::Vector2<T> &end, std::vector<Tile> &tileMap, unsigned width, unsigned height,
						 FastNoiseLite &noise, float noiseScale, T thick)
		{
			// Create line
			T dx = (end.x - start.x);
			T dy = (end.y - start.y);
			T length = std::sqrt(dx * dx + dy * dy);					   // Line length
			T stepSize = std::sqrt(2.0f) / 2.0f;						   // Step size
			int numSteps = static_cast<int>(std::ceil(length / stepSize)); // Number of steps

			std::vector<mygal::Vector2<T>> line(numSteps + 1);

			for (int i = 0; i <= numSteps; ++i)
			{
				T t = i / static_cast<float>(numSteps);
				T x = start.x + t * dx;
				T y = start.y + t * dy;
				line[i] = mygal::Vector2(x, y);
			}

			geomt::DeformLine(line, noise, noiseScale);

			mygal::Vector2<T> tangent = end - start;
			for (size_t i = 0; i < line.size(); i++)
			{
				const auto &point = line[i];
				if (i + 1 < line.size())
				{
					tangent = line[i + 1] - point;
				}

				auto perpendicular = mygal::normed(tangent.getOrthogonal());
				if (std::abs(thick - 1) < 0.01)
				{
					plotTile(static_cast<int>(point.x), static_cast<int>(point.y), width, height, tileMap);
				}
				else
				{
					mygal::Vector2<double> offsetUp = point + perpendicular * (thick / 2);
					mygal::Vector2<double> offsetDown = point - perpendicular * (thick / 2);

					T dTx = (offsetDown.x - offsetUp.x);
					T dTy = (offsetDown.y - offsetUp.y);

					int steps = static_cast<int>(std::ceil(offsetUp.getDistance(offsetDown) / stepSize));
					for (int s = 0; s <= steps; ++s)
					{
						T t = s / static_cast<T>(steps);
						T x = offsetUp.x + t * dTx;
						T y = offsetUp.y + t * dTy;
						plotTile(static_cast<int>(x), static_cast<int>(y), width, height, tileMap);
					}
				}
			}
		}

		template <typename T>
		struct LineData
		{
			mygal::Vector2<T> start;
			mygal::Vector2<T> end;

			LineData(const mygal::Vector2<T> &s, const mygal::Vector2<T> &e) : start(s), end(e) {};

			bool operator==(const LineData &other) const
			{
				return (start == other.start && end == other.end) || (start == other.end && end == other.start);
			}
		};
	}
}
namespace std
{
	template <typename T>
	struct hash<MapGenerator::rasterizer::LineData<T>>
	{
		std::size_t operator()(const MapGenerator::rasterizer::LineData<T> &line) const
		{
			size_t startHash = std::hash<mygal::Vector2<T>>()(line.start);
			size_t endHash = std::hash<mygal::Vector2<T>>()(line.end);

			// Combine hashes such that the order doesn't matter (e.g., addition or min/max approach)
			return startHash ^ endHash;
		}
	};
}

namespace MapGenerator
{
	namespace rasterizer
	{
		template <typename T>
		static void RasterizeDiagramToTileMap(const mygal::Diagram<T> &diagram, int width, int height, TileMap &tileMap, const NoiseData &noiseData, T borderThick)
		{
			std::vector<Tile> &tileMapVector = tileMap.GetTilesRef();
			std::unordered_set<LineData<T>> linesPlotted;

			for (const auto &halfEdge : diagram.getHalfEdges())
			{
				auto *start = &halfEdge;
				auto *it = &halfEdge;
				while (it != nullptr)
				{
					if (it->origin != nullptr && it->destination != nullptr)
					{
						auto origin = it->origin->point;
						auto destination = it->destination->point;

						if (!linesPlotted.contains(LineData(origin, destination)))
						{
							const mygal::Vector2<double> s = mygal::ScaleBothAxis(origin, static_cast<double>(width), static_cast<double>(height));
							const mygal::Vector2<double> e = mygal::ScaleBothAxis(destination, static_cast<double>(width), static_cast<double>(height));

							FastNoiseLite noise(noiseData.seed);
							ApplyNoiseData(noiseData, noise);
							Line(s, e, tileMapVector, width, height, noise, noiseData.scale, borderThick);

							linesPlotted.insert(LineData(origin, destination));
						}
					}
					it = it->next;
					if (it == start)
						break;
				}
			}
		}
	}
}
