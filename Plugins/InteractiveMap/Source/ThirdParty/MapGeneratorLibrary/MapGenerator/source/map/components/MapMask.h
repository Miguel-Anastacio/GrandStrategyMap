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
		MapMask(const char *name, const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height, ALogger::Logger &logger, float cutOffHeight, const bool mode = true);
		MapMask(const char *name, const std::vector<uint8_t> &textureBuffer, unsigned width, unsigned height, ALogger::Logger &logger, const bool mode = true);

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
		void RegenerateMask(const std::vector<uint8_t>& maskBuffer, const bool mode);

		const Mask &GetMask() const
		{
			return mask;
		}

	private:
		std::vector<double> ExtractHeightMapFromTexture(const std::vector<uint8_t> &buffer, unsigned width, unsigned height) const;
		std::vector<uint8_t> CreateBuffer(const std::vector<double> &data, float cutOffHeight, bool mode, uint8_t alpha = 0U) const;
		std::vector<uint8_t> CreateBuffer(const std::vector<uint8_t> &buffer, bool mode, uint8_t alpha = 0U) const;

		std::vector<double> m_elevation;
		std::vector<uint8_t> m_maskBuffer;
		Mask mask;
	};

}