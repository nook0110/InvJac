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
#include <phc/phcpack.h>
#include <phc/solcon.h>
#include <phc/syscon.h>

#include <cassert>
#include <cstddef>
#include <exception>
#include <iterator>
#include <span>
#include <sstream>
#include <thread>
#include <vector>

#include "map.hpp"
#include "phc_wrapper.hpp"
#include "point.hpp"

class Solver
{
 public:
  using Root = phc::Equation::Root;
  using Solution = phc::Equation::Solution;
  static Solver& GetInstance();

  Solution Solve(const Map& mapping, const Point& point) const;

  ~Solver() { adafinal(); }

 private:
  phc::Equation CreateEquation(const Map& mapping, const Point& point) const
  {
    LOG_IF(DFATAL, mapping.GetDimensions() != point.GetDimensions())
        << "Mapping and point dimensions are incompatible!";

    phc::Equation equation(mapping.GetDimensions());

    for (const auto& [polynomial, coordinate] : std::views::zip(mapping, point))
    {
      equation.AppendEquation(polynomial - coordinate);
    }

    return equation;
  }

  Solver() { adainit(); }
  Solver(const Solver& root) = delete;
  Solver(Solver&& root) = delete;
  Solver& operator=(const Solver&) = delete;
  Solver& operator=(Solver&&) = delete;
};