#pragma once

#include <cln/cln.h>
#include <cln/complex_class.h>
#include <cln/floatformat.h>
#include <cln/rational.h>
#include <cln/real.h>
#include <ginac/constant.h>
#include <ginac/ginac.h>
#include <ginac/numeric.h>
#include <glog/logging.h>
#include <cassert>

#include "equation.hpp"
#include "map.hpp"
#include "point.hpp"

class Solver
{
 public:
  using Root = Equation::Root;
  using Solution = Equation::Solution;
  static Solver& GetInstance();

  Solution Solve(const Map& mapping, const Point& point) const;

 private:
  Equation CreateEquation(const Map& mapping, const Point& point) const
  {
    LOG_IF(DFATAL, mapping.GetDimensions() != point.GetDimensions())
        << "Mapping and point dimensions are incompatible!";

    Equation equation(mapping.GetDimensions());

    for (const auto& [polynomial, coordinate] : std::views::zip(mapping, point))
    {
      equation.AppendEquation(polynomial - coordinate);
    }

    return equation;
  }

  Solver() = default;
  Solver(const Solver& root) = delete;
  Solver(Solver&& root) = delete;
  Solver& operator=(const Solver&) = delete;
  Solver& operator=(Solver&&) = delete;
};