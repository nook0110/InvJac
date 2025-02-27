#include <gtest/gtest.h>

#include <sstream>

#include "source/point_factory.hpp"

TEST(PointFactoryTest, CreateMapFromInput)
{
  std::stringstream input("1.0\n2.0\n");
  Point point = PointFactory::CreateMapFromInput(input, 2);

  EXPECT_EQ(point.ToStr(), "(1.0, 2.0)");
}
