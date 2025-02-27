#include <gtest/gtest.h>

#include <sstream>

#include "source/map_factory.hpp"

TEST(MapFactoryTest, CreateMapFromInput)
{
  std::stringstream input("2\nx^2\ny^2\n");
  std::stringstream output;
  Map map = MapFactory::CreateMapFromInput(input, output);

  EXPECT_EQ(map.GetDimensions(), 2);
  // Add more checks to verify the polynomials if necessary
}
