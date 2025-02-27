#include <gtest/gtest.h>

#include <sstream>

#include "source/point_factory.hpp"

TEST(PointFactoryTest, CreateMapFromInput)
{
  std::stringstream input(
      "1.0\n"
      "2.0\n");
  Point point = PointFactory::CreateMapFromInput(input, 2);

  EXPECT_EQ(point.GetCoord(0), 1.0);
  EXPECT_EQ(point.GetCoord(1), 2.0);
}
