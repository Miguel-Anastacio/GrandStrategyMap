#include "Color.h"
namespace MapGenerator
{
	namespace data
	{
		static const Color operator*(const Color &color, int scale)
		{
			return Color(color.R * scale, color.G * scale, color.B * scale, color.A);
		}

		std::ostream &operator<<(std::ostream &os, const Color &color)
		{
			os << "R:" << (int)color.R << " G:" << (int)color.G << " B:" << (int)color.B << " A:" << (int)color.A;
			return os;
		}
	}
}