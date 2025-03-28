#include <gtest/gtest.h>

#include "../source/point.hpp"

TEST(PointTest, DefaultConstructor)
{
  Point point;
  EXPECT_EQ(point.GetDimensions(), 0);
}

TEST(PointTest, ConstructorWithCoordinates)
{
  std::vector<Complex> coords = {1 + 2 * GiNaC::I, 3 + 4 * GiNaC::I};
  Point point(coords);
  EXPECT_EQ(point.GetDimensions(), 2);
  EXPECT_EQ(point[0], coords[0]);
  EXPECT_EQ(point[1], coords[1]);
}

TEST(PointTest, GenerateRandom)
{
  size_t dimensions = 3;
  Point point = Point::GenerateRandom(dimensions);
  EXPECT_EQ(point.GetDimensions(), dimensions);
}

TEST(PointTest, ToLst)
{
  std::vector<Complex> coords = {1 + 2 * GiNaC::I, 3 + 4 * GiNaC::I};
  Point point(coords);
  GiNaC::lst lst = point.ToLst();
  EXPECT_EQ(lst.nops(), coords.size());
  for (size_t i = 0; i < coords.size(); ++i)
  {
    EXPECT_EQ(lst.op(i), coords[i]);
  }
}

TEST(PointTest, ToStr)
{
  std::vector<Complex> coords = {1 + 2 * GiNaC::I, 3 + 4 * GiNaC::I};
  Point point(coords);
  std::string str = point.ToStr();
  EXPECT_EQ(str, "(1.0+2.0*I, 3.0+4.0*I)");
}

TEST(PointTest, Iterators)
{
  std::vector<Complex> coords = {1 + 2 * GiNaC::I, 3 + 4 * GiNaC::I};
  Point point(coords);
  auto it = point.begin();
  for (const auto& coord : coords)
  {
    EXPECT_EQ(*it, coord);
    ++it;
  }
  EXPECT_EQ(it, point.end());
}
