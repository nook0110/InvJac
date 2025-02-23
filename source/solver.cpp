#include "solver.hpp"

#include <bits/ranges_algo.h>
#include <cln/real.h>
#include <complex.h>

#include "map.hpp"
#include "phc_wrapper.hpp"
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
