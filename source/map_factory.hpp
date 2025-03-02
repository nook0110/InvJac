#pragma once

#include <istream>
#include <ostream>

#include "map.hpp"

/**
 * @brief Factory class to create Map objects.
 */
class MapFactory
{
 public:
  /**
   * @brief Creates a Map object from input streams.
   * @param input Input stream to read the map data from.
   * @param out Output stream to write any messages or errors.
   * @return Created Map object.
   */
  static Map CreateMapFromInput(std::istream& input, std::ostream& out);
};
