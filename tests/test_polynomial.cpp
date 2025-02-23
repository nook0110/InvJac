#include <gtest/gtest.h>

#include "source/point.hpp"
#include "source/polynomial.hpp"

static const auto& x = Symbols::GetSymbol(0);
static const auto& y = Symbols::GetSymbol(1);

TEST(MapTest, ImageTest)
{
  Map map({x + y, x - y});
  Point point({1, 2});
  Point image = map.Image(point);
  EXPECT_EQ(image.GetCoord(0), 3);
  EXPECT_EQ(image.GetCoord(1), -1);
}

TEST(MapTest, GetJacobianMatrixTest)
{
  Map map({x * y, x + y});
  auto jacobian_matrix = map.GetJacobianMatrix();

  EXPECT_EQ(jacobian_matrix(0, 0), y);
  EXPECT_EQ(jacobian_matrix(0, 1), x);
  EXPECT_EQ(jacobian_matrix(1, 0), 1);
  EXPECT_EQ(jacobian_matrix(1, 1), 1);
}

TEST(MapTest, GetJacobianTest)
{
  Map map({x * y, x + y});
  auto jacobian = map.GetJacobian();
  EXPECT_EQ(jacobian, y - x);
}

TEST(MapTest, HasContractionTest)
{
  Map map_without_contraction({x * y, x + y});
  EXPECT_FALSE(map_without_contraction.HasContraction());

  Map map_with_contraction({x * x, x * y});
  EXPECT_TRUE(map_with_contraction.HasContraction());
}
