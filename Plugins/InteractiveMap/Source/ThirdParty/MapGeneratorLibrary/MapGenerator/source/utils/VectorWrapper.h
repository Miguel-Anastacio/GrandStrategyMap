#pragma once
#include "../../ThirdParty/MyGAL/Vector2.h"
#include <functional>
#include <iostream>

namespace mygal
{
	template <typename T>
	inline mygal::Vector2<T> normed(const mygal::Vector2<T> &vector)
	{
		auto norm = vector.getNorm();
		return mygal::Vector2<T>(vector.x / norm, vector.y / norm);
	}

	template <typename T>
	inline mygal::Vector2<T> operator/(const mygal::Vector2<T> &vector, T div)
	{
		return mygal::Vector2<T>(vector.x / div, vector.y / div);
	}

	template <typename T>
	inline mygal::Vector2<T> operator*(const mygal::Vector2<T> &vector, const mygal::Vector2<T> &scale)
	{
		return mygal::Vector2<T>(vector.x * scale.x, vector.y * scale.y);
	}

	template <typename T>
	inline mygal::Vector2<T> operator/(const mygal::Vector2<T> &vector, const mygal::Vector2<T> &scale)
	{
		return mygal::Vector2<T>(vector.x / scale.x, vector.y / scale.y);
	}

	template <typename T>
	static void operator/=(mygal::Vector2<T> &vector, T scale)
	{
		vector.x /= scale;
		vector.y /= scale;
	}

	template <typename T, typename K>
	static mygal::Vector2<T> ScaleBothAxis(const mygal::Vector2<T> &og, K xScale, K yScale)
	{
		return mygal::Vector2<T>(og.x * xScale, og.y * yScale);
	}
}

namespace std
{
	template <typename T>
	struct hash<mygal::Vector2<T>>
	{
		inline std::size_t operator()(const mygal::Vector2<T> &point) const
		{
			return std::hash<T>()(point.x) ^ (std::hash<T>()(point.y));
		}
	};
}
