#pragma once
#include <vector>
#include <cassert>
#include <string>
#include "../MapComponent.h"
#include "../MapSpecs.h"
#include "../../data/Mask.h"
namespace MapGenerator
{
	using namespace data;
	class MapMask : public MapComponent
	{
	public:
		MapMask(const char *name, const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height, float cutOffHeight, const bool mode = true);
		// HeightMap(const Texture& texture, double noiseScale, const siv::PerlinNoise& noise, const NoiseSpecs& specs);

		inline std::vector<double> MoveElevation()
		{
			return std::move(m_elevation);
		}
		inline std::vector<double> GetElevation()
		{
			return m_elevation;
		}

		inline std::vector<uint8_t> GetMaskBuffer() const
		{
			return m_maskBuffer;
		}

		void RegenerateMask(float cutOffHeight, bool mode = true);

		const Mask &GetMask() const
		{
			return mask;
		}

	private:
		// std::vector<double> CreateHeightMap(const NoiseMapData& data) const;
		std::vector<double> ExtractHeightMapFromTexture(const std::vector<uint8_t> &buffer, unsigned width, unsigned height) const;
		std::vector<uint8_t> CreateBuffer(const std::vector<double> &data, float cutOffHeight, bool mode, uint8_t alpha = 0U) const;

		std::vector<double> m_elevation;
		std::vector<uint8_t> m_maskBuffer;
		Mask mask;
	};

}