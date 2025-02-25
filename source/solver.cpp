#include "solver.hpp"

#include <complex.h>

#include "map.hpp"
#include "equation.hpp"
#include "point.hpp"

Solver& Solver::GetInstance()
{
  static Solver instance;
  return instance;
}

Solver::Solution Solver::Solve(const Map& mapping, const Point& point) const
{
  return CreateEquation(mapping, point).Solve();
}
