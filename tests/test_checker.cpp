#include <gtest/gtest.h>

#include "source/checker.hpp"
#include "source/map.hpp"
#include "source/point.hpp"
#include "source/symbol.hpp"

static const auto& x = Symbols::GetSymbol(0);
static const auto& y = Symbols::GetSymbol(1);

TEST(CheckerTest, SetMapAndPerformCheck)
{
  CheckerSettings settings;
  settings.iterations = 5;
  settings.epsilon = 1e-3;

  Checker checker(settings);
  Map map{{x * x + 1, y * y - 1}};
  checker.SetMap(&map);

  auto result = checker.PerformCheck();
  EXPECT_EQ(result.index(), 2);
}

TEST(CheckerTest, PerformCheckWithContraction)
{
  CheckerSettings settings;
  settings.check_contraction = true;

  Checker checker(settings);
  Map map{{x * x + 1, y * y - 1}};
  checker.SetMap(&map);

  auto result = checker.PerformCheck();
  EXPECT_EQ(result.index(), 2);
}

TEST(CheckerTest, GeneratePointWithSpecificJacobian)
{
  Point point({1, 1});
  Map map{{x * x + 1, y * y - 1}};
  Complex value(1);

  auto generated_point = GeneratePointWithSpecificJacobian(map, point, value);
  EXPECT_TRUE(generated_point.has_value());
  EXPECT_EQ(generated_point->ToStr(), "(0.25, 1.0)");
}
