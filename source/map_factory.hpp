#pragma once

#include <istream>

#include "map.hpp"

class MapFactory
{
 public:
  static Map CreateMapFromInput(std::istream& input, std::ostream& out);
};
