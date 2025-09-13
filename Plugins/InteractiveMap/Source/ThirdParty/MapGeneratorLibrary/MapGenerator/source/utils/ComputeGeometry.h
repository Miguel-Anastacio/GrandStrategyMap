#pragma once
#include <vector>
#include <random>
#include <chrono>
#include "../../ThirdParty/MyGAL/Diagram.h"
#include "../../ThirdParty/MyGAL//FortuneAlgorithm.h"
#include "../data/Mask.h"
#include <cmath>
#include <stack>
#include <set>
#include <iostream>
#include "../../ThirdParty/fastNoiseLite/FastNoiseLite.h"
#include "VectorWrapper.h"
namespace MapGenerator
{
	namespace geomt
	{
		// ------------------------ MYGAL
		template <typename T>
		static std::vector<mygal::Vector2<T>> generatePoints(int nbPoints)
		{
			auto seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::cout << "seed: " << seed << '\n';
			auto generator = std::default_random_engine(seed);
			auto distribution = std::uniform_real_distribution<T>(0.0, 1.0);

			auto points = std::vector<mygal::Vector2<T>>(nbPoints);
			for (auto i = 0; i < nbPoints; ++i)
				points[i] = mygal::Vector2<T>(distribution(generator), distribution(generator));

			return points;
		}
		template <typename T>
		static std::vector<mygal::Vector2<T>> generatePoints(int nbPoints, int seedValue)
		{
			std::cout << "seed: " << seedValue << '\n';
			auto generator = std::default_random_engine(seedValue);
			auto distribution = std::uniform_real_distribution<T>(0.0, 1.0);

			auto points = std::vector<mygal::Vector2<T>>(nbPoints);
			for (auto i = 0; i < nbPoints; ++i)
				points[i] = mygal::Vector2<T>(distribution(generator), distribution(generator));

			return points;
		}

		template <typename T>
		static std::vector<mygal::Vector2<T>> generatePointsConstrained(int nbPoints, int seedValue, bool state, const Mask &mask)
		{
			// Timer
			std::cout << "seed: " << seedValue << '\n';
			std::default_random_engine generator(seedValue);
			std::uniform_real_distribution<T> distribution(0.0, 1.0);

			std::vector<mygal::Vector2<T>> points;
			points.reserve(nbPoints);

			const int maxAttempts = 500;
			int attempts = 0;

			while (points.size() < nbPoints)
			{
				T x = distribution(generator);
				T y = distribution(generator);
				int maskX = static_cast<int>(x * mask.Width());
				int maskY = static_cast<int>(y * mask.Height());

				if (mask.isInMask(maskX, maskY))
				{
					points.emplace_back(x, y);
					attempts = 0;
				}
				else
				{
					attempts++;
					if (attempts > maxAttempts)
						return points;
				}
			}

			return points;
		}

		template <typename T>
		static mygal::Diagram<T> generateDiagram(const std::vector<mygal::Vector2<T>> &points)
		{
			// Construct diagram
			auto algorithm = mygal::FortuneAlgorithm<T>(points);
			algorithm.construct();

			// Bound the diagram
			algorithm.bound(mygal::Box<T>{-0.05, -0.05, 1.05, 1.05}); // Take the bounding box slightly bigger than the intersection box
			auto diagram = algorithm.getDiagram();

			// Intersect the diagram with a box
			diagram.intersect(mygal::Box<T>{-0.01, -0.01, 1.01, 1.01});

			return diagram;
		}
		// Helper: Check if point is inside face (using winding number or ray casting)
		template <typename T>	
		static bool isPointInFace(const mygal::Vector2<T>& point, const typename mygal::Diagram<T>::Face& face)
		{
			// Ray casting algorithm
			int crossings = 0;
			auto halfEdge = face.outerComponent;
    
			do {
				mygal::Vector2<T> p1 = halfEdge->origin->point;
				mygal::Vector2<T> p2 = halfEdge->destination->point;
        
				if (((p1.y <= point.y) && (point.y < p2.y)) || 
					((p2.y <= point.y) && (point.y < p1.y))) {
            
					T intersectX = p1.x + (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
					if (point.x < intersectX) {
						crossings++;
					}
					}
				halfEdge = halfEdge->next;
			} while (halfEdge != face.outerComponent);
    
			return (crossings % 2) == 1;
		}

		template <typename T>	
		static mygal::Vector2<T> FindNearestValidPoint(const mygal::Vector2<T>& point, const Mask& mask)
		{
			auto wid = static_cast<int>(mask.Width());
			auto hgt = static_cast<int>(mask.Height());
			const int x = point.x * wid;
			const int y = point.y * hgt;
			if(mask.isInMask(x, y))
			{
				return point;
			}

			if (x >= wid || x < 0 || y >= hgt || y < 0)
			{
				return point;
			}

			// Stack for iterative approach
			std::stack<std::pair<int, int>> stack;
			std::set<std::pair<int, int>> visitedPoints;
			stack.push({x, y});

			while (!stack.empty())
			{
				auto [cx, cy] = stack.top();
				stack.pop();
				visitedPoints.insert({cx, cy});
				// Check boundary conditions again and ensure tile hasn't been visited
				if (cx < 0 || cx >= wid || cy < 0 || cy >= hgt)
					continue;
				
				if(visitedPoints.contains({cx, cy}))
				{
					continue;;
				}

				if(mask.isInMask(cx, cy))
				{
					return mygal::Vector2<T>(cx/wid, cy/hgt);
				}

				// Push neighboring tiles onto the stack
				stack.push({cx + 1, cy});
				stack.push({cx - 1, cy});
				stack.push({cx, cy + 1});
				stack.push({cx, cy - 1});
			}

			return point;
		}
		
		// Helper: Extract vertices from face
		template <typename T>	
		static std::vector<mygal::Vector2<T>> getFaceVertices(const typename mygal::Diagram<T>::Face& face)
		{
		    std::vector<mygal::Vector2<T>> vertices;
		    auto halfEdge = face.outerComponent;
		    
		    do {
		        vertices.push_back(halfEdge->origin->point);
		        halfEdge = halfEdge->next;
		    } while (halfEdge != face.outerComponent);
		    
		    return vertices;
		}

		// Method 2: Grid sampling approach (simpler, less accurate)
		template <typename T>	
		static mygal::Vector2<T> computeGridSampledCentroid(const typename mygal::Diagram<T>::Face& face, const Mask& mask)
		{
			// Get bounding box of face
			auto [minX, minY, maxX, maxY] = getFaceBounds<T>(face);

			T totalWeight = 0.0;
			bool pointInside = false;
			mygal::Vector2<T> weightedCentroid(0, 0);

			// Sample resolution - higher = more accurate but slower
			const T sampleStep = 0.05; // Adjust based on your coordinate system

			for (T y = minY; y <= maxY; y += sampleStep) {
				for (T x = minX; x <= maxX; x += sampleStep) {
					mygal::Vector2<T> samplePoint(x, y);
	        
					// Check if point is inside face AND inside mask
					int xScaled = x * mask.Width();
					int yScaled = y * mask.Height();
					if (isPointInFace(samplePoint, face) && mask.isInMask(xScaled, yScaled))
					{
						// Weight by sample area (sampleStep^2)
						T weight = sampleStep * sampleStep;
						weightedCentroid += samplePoint * weight;
						totalWeight += weight;
						pointInside = true;
					}
				}
			}

			if (totalWeight > 0 && pointInside)
			{
				std::cout << "Point in face and mask, weight: "<< totalWeight << std::endl;
				return weightedCentroid / totalWeight;
			}

			// Fallback: return closest valid point to original centroid
			auto originalCentroid = mygal::Diagram<T>::computeCentroidOfFace(face);
			std::cout << "Point using fallback" << std::endl;
			return FindNearestValidPoint(originalCentroid, mask);
		}
		
		// Helper: Get face bounding box
		template <typename T>	
		static std::tuple<T, T, T, T> getFaceBounds(const typename mygal::Diagram<T>::Face& face)
		{
			T minX = std::numeric_limits<T>::max();
			T minY = std::numeric_limits<T>::max();
			T maxX = std::numeric_limits<T>::lowest();
			T maxY = std::numeric_limits<T>::lowest();
    
			auto halfEdge = face.outerComponent;
			do {
				const auto& point = halfEdge->origin->point;
				minX = std::min(minX, point.x);
				minY = std::min(minY, point.y);
				maxX = std::max(maxX, point.x);
				maxY = std::max(maxY, point.y);
				halfEdge = halfEdge->next;
			} while (halfEdge != face.outerComponent);
    
			return {minX, minY, maxX, maxY};
		}
		
		template <typename T>	
		static std::vector<mygal::Vector2<T>> computeMaskedLloydRelaxation(const Mask& mask, mygal::Diagram<T> &diagram)
		{
			auto sites = std::vector<mygal::Vector2<T>>();
			auto faces = diagram.getFaces();
			sites.reserve(faces.size());
		    int index = 0;
			for (const typename mygal::Diagram<T>::Face& face : faces)
			{
				std::cout << "Face: " << index++ << std::endl;
				mygal::Vector2<T> maskedCentroid = computeGridSampledCentroid<T>(face, mask);
				std::cout << "Centroid: " << maskedCentroid.x << ":" << maskedCentroid.y << std::endl;
				std::cout << "-----------------" << std::endl;
				sites.push_back(maskedCentroid);
			}
			return sites;
		}

		template <typename T>
		static void lloydRelaxation(mygal::Diagram<T> &diagram, int iterations, const Mask &mask)
		{
			for (int i = 0; i < iterations; i++)
			{
				// std::vector<mygal::Vector2<T>> centroids = computeMaskedLloydRelaxation<T>(mask, diagram);
				std::vector<mygal::Vector2<T>> centroids = computeMaskedLloydRelaxation<T>(mask, diagram);
				// std::vector<mygal::Vector2<T>> centroids = diagram.computeLloydRelaxation();
				diagram = std::move(geomt::generateDiagram(centroids));
			}
		}

		template <typename T>
		static void lloydRelaxation(mygal::Diagram<T> &diagram, int iterations)
		{
			for (int i = 0; i < iterations; i++)
			{
				std::vector<mygal::Vector2<double>> centroids = diagram.computeLloydRelaxation();
				diagram = std::move(geomt::generateDiagram(centroids));
			}
		}

		template <typename T>
		static std::vector<mygal::Vector2<T>> GetCentroidsOfDiagram(mygal::Diagram<T> &diagram)
		{
			std::vector<mygal::Vector2<T>> centroids;
			centroids.reserve(diagram.getSites().size());
			for (auto &site : diagram.getSites())
			{
				centroids.emplace_back(site.point);
			}
			return centroids;
		}

		// ------------------------ MYGAL

		template <typename T>
		static std::vector<T> sampleNoiseInACircle(const FastNoiseLite &noise, float radius, int numSamples, float offsetX = 0.0f, float offsetY = 0.0f)
		{
			std::vector<T> circleNoise(numSamples);
			for (int i = 0; i < numSamples; ++i)
			{
				// Sample at different angles around the circle
				T theta = (2 * 3.14159265359f * i) / numSamples;
				T x = radius * cos(theta) + offsetX;
				T y = radius * sin(theta) + offsetY;

				// Sample 2D noise
				circleNoise[i] = noise.GetNoise(x, y);
			}
			return circleNoise;
		}

		// Function to normalize the noise values
		template <typename T>
		static void normalizeNoise(std::vector<T> &noise, int cutPointIndex)
		{
			T cutPointValue = noise[cutPointIndex];
			for (auto &val : noise)
			{
				val -= cutPointValue; // Normalize to make noise at cut point zero
			}
		}

		// Function to map circle noise to a 1D fault line
		template <typename T>
		static void applyNoiseToLine(const std::vector<T> &circleNoise, std::vector<mygal::Vector2<T>> &line, int cutPointIndex, float noiseScale, int thickness)
		{
			auto numSamples = circleNoise.size();
			std::vector<T> faultLine(numSamples);

			// Map circle noise to fault line, with cut point at both ends

			faultLine[0] = circleNoise[cutPointIndex];
			faultLine[numSamples - 1] = circleNoise[cutPointIndex];
			for (int i = 1; i < numSamples - 1; ++i)
			{
				faultLine[i] = circleNoise[i];
			}

			int index = 0;
			for (int i = 0; i < numSamples; ++i)
			{
				mygal::Vector2<T> tangent = line.size() > 1 ? line[numSamples - 1] - line[0] : mygal::Vector2<T>(1, 0); // Approximate tangent
				auto perpendicular = mygal::normed(tangent.getOrthogonal());

				line[i].x += faultLine[i] * noiseScale * perpendicular.x;
				line[i].y += faultLine[i] * noiseScale * perpendicular.y;
			}
		}

		template <typename T>
		static void DeformLine(std::vector<mygal::Vector2<T>> &line, FastNoiseLite &noise, float noiseScale = 25.0f, int thickness = 2)
		{
			// Initialize the noise generator
			// int seed = std::rand();
			noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

			// Parameters
			float radius = 1.0f;			 // Circle radius
			size_t numSamples = line.size(); // Number of samples (points on the circle)
			const int maxvalue = INT32_MAX;
			int cutPointIndex = 0; // Cut point (can be any index on the circle)

			// Step 1: Sample noise on the circle
			std::vector<double> circleNoise = sampleNoiseInACircle<double>(noise, radius, (int)numSamples);

			// Step 2: Normalize noise so that the value at the cut point is zero
			normalizeNoise(circleNoise, cutPointIndex);

			// Step 3: Map circle noise to a fault line
			applyNoiseToLine(circleNoise, line, cutPointIndex, noiseScale, thickness);
		}
	}
}