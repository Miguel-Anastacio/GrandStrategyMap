#pragma once
#include "HeightMap.h"
#include <algorithm>
#include "../../../thirdParty/fastNoiseLite/FastNoiseLite.h"
#include "../../../thirdParty/MyGAL/Vector2.h"
#include "../../graphics/Rasterizer.h"
#include <execution>
#include <thread>
#include <numeric> // for std::transform
#include "../../utils/ComputeGeometry.h"
namespace MapGenerator
{
	HeightMap::HeightMap(const char *name, const NoiseMapData &data)
		: MapComponent(1024, 512, name)
	{
		// RegenerateHeightMap(data);
		CreateHeightTectonicPlates(data);
		// rend::drawBuffer(CreateBuffer(), m_texture, this->Width(), this->Height());
	}

	HeightMap::HeightMap(const char *name, unsigned width, unsigned height, std::vector<double> &&elevation)
		: MapComponent(width, height, name)
	{
		SetNoiseMap(std::move(elevation));
		CreateHeightTectonicPlates();
	}

	HeightMap::~HeightMap()
	{
	}

	void HeightMap::SetNoiseMap(std::vector<double> &&elevation)
	{
		m_elevation = std::move(elevation);
		// rend::drawBuffer(CreateBuffer(), m_texture, this->Width(), this->Height());
	}

	void HeightMap::RegenerateHeightMap(const NoiseMapData &data)
	{
		/*auto start = std::chrono::steady_clock::now();
		m_elevation = CreateHeightMapFromNoise(data);

		auto duration = std::chrono::steady_clock::now() - start;
		std::cout << "Noise generator: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';*/

		// SaveHeightMapToFile();
	}

	std::vector<double> HeightMap::CreateHeightMapFromNoise(const NoiseMapData &data) const
	{
		float scaleFactor = 1.0f / data.scale;

		float minNoise = std::numeric_limits<double>::max();
		float maxNoise = std::numeric_limits<double>::min();

		// Number of threads to use
		std::vector<double> heightMap(data.width * data.height);
		const int num_threads = 8;
		std::vector<std::thread> threads;

		//// Calculate the size of each chunk
		//const int sliceSize = data.height / num_threads;
		//std::vector<mygal::Vector2<double>> minMax(num_threads);

		//// Lambda function to process a section of the map
		//auto process_chunk = [&heightMap, scaleFactor, data, &minMax](int startY, int endY, int index)
		//{
		//	FastNoiseLite noise;
		//	noise.SetSeed(data.seed);
		//	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		//	noise.SetFrequency(data.frequency);
		//	noise.SetFractalOctaves(data.octaves);
		//	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		//	noise.SetFractalLacunarity(data.lacunarity);
		//	noise.SetFractalGain(0.5f);

		//	for (int y = startY; y < endY; ++y)
		//	{
		//		for (int x = 0; x < data.width; ++x)
		//		{
		//			const double xSample = (float)x * scaleFactor;
		//			const double ySample = (float)y * scaleFactor;
		//			const double noiseVal = noise.GetNoise(xSample, ySample) * 0.5 + 0.5;
		//			heightMap[y * data.width + x] = noiseVal;

		//			if (noiseVal > minMax[index].y)
		//				minMax[index].y = noiseVal;

		//			if (noiseVal < minMax[index].x)
		//				minMax[index].x = noiseVal;
		//		}
		//	}
		//};

		//// Start threads
		//for (int i = 0; i < num_threads; ++i)
		//{
		//	threads.emplace_back(std::thread(process_chunk, i * sliceSize, (i + 1) * sliceSize, i));
		//}

		//// Join threads
		//for (auto &t : threads)
		//{
		//	t.join();
		//}

		//// calcl min and max based on min and max values in the other chunks
		//for (auto &localMinMax : minMax)
		//{
		//	if (localMinMax.x < minNoise)
		//		minNoise = localMinMax.x;
		//	if (localMinMax.y > maxNoise)
		//		maxNoise = localMinMax.y;
		//}

		//// could also use threads for this
		///*for (int y = 0; y < data.height; y++)
		//{
		//	for (int x = 0; x < data.width; x++)
		//	{
		//		heightMap[y * data.width + x] = inverseLerp(minNoise, maxNoise, heightMap[y * data.width + x]);
		//	}
		//}*/

		return heightMap;
	}

	void HeightMap::CreateHeightTectonicPlates(const NoiseMapData &data)
	{
		int seed = 0;
		int lloyd = 10;
		int points = 50;

		auto pointsContr = geomt::generatePoints<double>(points, seed);
		mygal::Diagram<double> diagram = std::move(geomt::generateDiagram(pointsContr));
		geomt::lloydRelaxation(diagram, lloyd);

		/*auto tileMap = rasterizer::(diagram, Width(), Height(), data.scale);

		std::vector<mygal::Vector2<double>> centroids;
		centroids.reserve(diagram.getSites().size());
		for (auto& site : diagram.getSites())
		{
			centroids.emplace_back(site.point);
		}

		algo::floodFill(tileMap, centroids, Width(), Height());

		m_texture.clear();
		rend::drawTileMap(m_texture, tileMap, Width(), Height());
		rend::saveToFile(m_texture, "testweekacs.png");*/
	}

	void HeightMap::CreateHeightTectonicPlates()
	{
		/*	int seed = 0;
			int lloyd = 10;
			int points = 50;

			auto pointsContr = geomt::generatePoints<double>(points, seed);
			mygal::Diagram<double>  diagram = std::move(geomt::generateDiagram(pointsContr));
			geomt::lloydRelaxation(diagram, lloyd);

			auto tileMap = rasterizer::CreateTileFromDiagram(diagram, Width(), Height());
			m_texture.clear();
			rend::drawTileMap(m_texture, tileMap, Width(), Height());
			rend::saveToFile(m_texture, "testweekacs.png");*/
	}

	std::vector<uint8_t> HeightMap::CreateBuffer()
	{
		unsigned width = this->Width();
		unsigned height = this->Height();
		std::vector<uint8_t> image(width * height * 4);
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				double value = m_elevation[width * y + x];
				image[4 * width * y + 4 * x + 0] = value * 255;
				image[4 * width * y + 4 * x + 1] = value * 255;
				image[4 * width * y + 4 * x + 2] = value * 255;
				image[4 * width * y + 4 * x + 3] = 255;
			}
		}
		return image;
	}

}