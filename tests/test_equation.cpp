#include <ginac/ginac.h>
#include <ginac/symbol.h>
#include <gtest/gtest.h>

#include <cassert>

#include "source/equation.hpp"
#include "source/point.hpp"
#include "source/symbol.hpp"

static const auto& x = Symbols::GetSymbol(0);
static const auto& y = Symbols::GetSymbol(1);

bool IsNear(const Point& first, const Point& second)
{
  assert(first.GetDimensions() == second.GetDimensions());
  for (size_t i = 0; i < first.GetDimensions(); ++i)
  {
    if (!(GiNaC::abs(first[i] - second[i]) < 1e-5))
    {
      return false;
    }
  }
  return true;
}

TEST(EquationTest, IsSquare)
{
  Equation equation(2);
  equation.AppendEquation(x * y - 2);
  equation.AppendEquation(x * x - 1);
  ASSERT_TRUE(equation.IsSquare());
}

TEST(EquationTest, Solve)
{
  Equation equation(2);
  equation.AppendEquation(x * y - 2);
  equation.AppendEquation(x * x - 1);
  auto solution = equation.Solve();
  ASSERT_EQ(solution.GetRootCount<false>(), 2);
  ASSERT_EQ(solution.GetRootCount<true>(), 2);

  const auto first_solution = Point({1, 2});
  const auto second_solution = Point({-1, -2});
  for (const auto& [multiplicity, root] : solution)
  {
    ASSERT_EQ(multiplicity, 1);
    ASSERT_TRUE(IsNear(root, first_solution) || IsNear(root, second_solution));
  }
}
