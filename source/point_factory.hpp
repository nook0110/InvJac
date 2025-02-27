#pragma once

#include <istream>

#include "point.hpp"

class PointFactory {
public:
 static Point CreateMapFromInput(std::istream& stream, size_t dimensions);
};
