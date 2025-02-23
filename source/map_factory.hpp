#pragma once

#include <iostream>
#include <vector>

#include "map.hpp"

class MapFactory
{
 public:
  static Map CreateMapFromInput(std::istream& input);
};
