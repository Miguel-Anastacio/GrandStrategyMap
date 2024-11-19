#pragma once
#include <vector>
#include "Color.h"
#include <queue>
#include "Utils.h"

namespace MapGenerator
{
	class Mask : public data::Dimensions
	{
	public:
		Mask(unsigned wid, unsigned ht, const std::vector<uint8_t> &textureBuffer, const data::Color &maskColor = data::Color(255, 255, 255, 255))
			: Dimensions(wid, ht), data(wid * ht)
		{
			SetMaskData(textureBuffer, maskColor);
		}

		Mask(unsigned wid, unsigned ht, const data::Color &maskColor = data::Color(255, 255, 255, 255))
			: Dimensions(wid, ht), data(wid * ht)
		{
		}

		bool isInMask(int x, int y) const;

		void SetMaskData(const std::vector<uint8_t> &buffer, const data::Color &maskColor);

	private:
		std::vector<bool> data;
	};

}