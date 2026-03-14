#pragma once
#include "../data/Utils.h"
#include "../utils/Logger/Logger.h"

namespace MapGenerator
{
	class MapComponent : public data::Dimensions
	{
	public:
		MapComponent(unsigned width, unsigned height, const char *name, ALogger::Logger& logger)
		: Dimensions(width, height),
		  m_name(name),
		  m_logger(logger)
		{};
		virtual ~MapComponent() = default;
		const char *Name() const
		{
			return m_name;
		}

		void Clear()
		{
		}

	protected:
		const char *m_name;
		ALogger::Logger& m_logger;
	};

}
