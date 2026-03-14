#include "Utils.h"
#include <unordered_set>
namespace MapGenerator
{
	namespace data
	{
		Color GetRandomColorNotInSet(const std::unordered_set<data::Color> &set)
		{
			data::Color color;
			do
			{
				color.RandColor();
			} while (set.contains(color) || color == data::Color(255, 255, 255, 255));

			return color;
		}
	}
}
