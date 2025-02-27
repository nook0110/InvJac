#pragma once

#include <istream>

#include "point.hpp"

class PointFactory
{
 public:
  static Point CreatePointFromInput(std::istream& input, std::ostream& output,
                                    size_t dimensions);
};
