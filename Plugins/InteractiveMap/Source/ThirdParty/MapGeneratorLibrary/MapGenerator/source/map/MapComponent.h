#pragma once
#include "../data/Utils.h"
namespace MapGenerator
{
	class MapComponent : public data::Dimensions
	{
	public:
		MapComponent(unsigned width, unsigned height, const char *name) : Dimensions(width, height), m_name(name) 
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
	};

}