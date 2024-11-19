#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Color.h"
#include <unordered_set>
namespace MapGenerator
{
    namespace data
    {
        class Dimensions
        {
        public:
            Dimensions(unsigned width, unsigned height) : m_width(width), m_height(height) {};
            Dimensions() : m_width(1), m_height(1) {};

            inline unsigned Width() const
            {
                return m_width;
            }
            inline unsigned Height() const
            {
                return m_height;
            }

        protected:
            inline void setDimensions(unsigned width, unsigned height)
            {
                m_width = width;
                m_height = height;
            }

        private:
            unsigned m_width;
            unsigned m_height;
        };

        Color GetRandomColorNotInSet(const std::unordered_set<data::Color>& set);

    } // namespace data

} // namespace MapGeneratorTool
