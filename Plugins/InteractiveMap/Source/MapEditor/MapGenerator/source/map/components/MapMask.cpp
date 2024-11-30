#pragma once
#include "MapMask.h"
#include <algorithm>
#include "../../../thirdParty/MyGAL/Vector2.h"

namespace MapGenerator
{
	MapMask::MapMask(const char *name, const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height, float cutOffHeight, bool mode)
		: MapComponent(width, height, name), mask(width, height)
	{
		m_elevation = ExtractHeightMapFromTexture(textureBuffer, width, height);
		m_maskBuffer = CreateBuffer(m_elevation, cutOffHeight, mode);
		mask.SetMaskData(m_maskBuffer, data::Color(255, 255, 255, 255));
	}

	std::vector<uint8_t> MapMask::CreateBuffer(const std::vector<double> &data, float cutOffHeight, bool mode, uint8_t alpha) const
	{
		unsigned width = this->Width();
		unsigned height = this->Height();
		std::vector<uint8_t> image(width * height * 4);
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				double value = data[width * y + x];

				bool condition = mode ? (value > cutOffHeight) : (value < cutOffHeight);
				uint8_t pixelValue = condition ? 255 : 0;
				uint8_t alphaValue = condition ? 255 : alpha;

				image[4 * width * y + 4 * x + 0] = pixelValue; // Red
				image[4 * width * y + 4 * x + 1] = pixelValue; // Green
				image[4 * width * y + 4 * x + 2] = pixelValue; // Blue
				image[4 * width * y + 4 * x + 3] = alphaValue; // Alpha
			}
		}
		return image;
	}

	void MapMask::RegenerateMask(float cutOffHeight, bool mode)
	{
		m_maskBuffer = CreateBuffer(m_elevation, cutOffHeight, mode);
		mask.SetMaskData(m_maskBuffer, data::Color(255, 255, 255, 255));
	}

	std::vector<double> MapMask::ExtractHeightMapFromTexture(const std::vector<uint8_t> &buffer, unsigned width, unsigned height) const
	{
		// assert(buffer.size() == (width * height * 4);

		std::vector<double> elevation(width * height);
		for (unsigned y = 0; y < height; y++)
		{
			for (unsigned x = 0; x < width; x++)
			{
				int index = 4 * width * y + 4 * x;

				elevation[width * y + x] = static_cast<double>(buffer[index]) / 255.f;
			}
		}
		return elevation;
	}

}