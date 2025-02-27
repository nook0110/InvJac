#include <gtest/gtest.h>

#include <sstream>

#include "source/map_factory.hpp"
#include "source/symbol.hpp"

const auto& x = Symbols::GetSymbol(0);
const auto& y = Symbols::GetSymbol(1);

TEST(MapFactoryTest, CreateMapFromInput)
{
  std::stringstream input(
      "2\n"
      "x^2\n"
      "y^2\n");
  std::stringstream output;
  Map map = MapFactory::CreateMapFromInput(input, output);

  EXPECT_EQ(map.GetDimensions(), 2);
  EXPECT_EQ(map.GetPoly(0), x * x);
  EXPECT_EQ(map.GetPoly(1), y * y);
}
