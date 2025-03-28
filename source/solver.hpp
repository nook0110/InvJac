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

/**
 * @class Solver
 * @brief A class for solving equations defined by mappings and points.
 */
class Solver
{
 public:
  using Root = Equation::Root;          ///< Alias for the root type.
  using Solution = Equation::Solution;  ///< Alias for the solution type.

  /**
   * @brief Gets the singleton instance of the Solver.
   * @return A reference to the Solver instance.
   */
  static Solver& GetInstance();

  /**
   * @brief Solves the equation defined by the given mapping and point.
   * @param mapping The mapping defining the equations.
   * @param point The point defining the equations.
   * @return The solution to the equations.
   */
  Solution Solve(const Map& mapping, const Point& point) const;

 private:
  /**
   * @brief Creates an equation from the given mapping and point.
   * @param mapping The mapping defining the equations.
   * @param point The point defining the equations.
   * @return The constructed Equation object.
   */
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

  /**
   * @brief Constructs a Solver instance.
   */
  Solver() = default;

  Solver(const Solver& root) = delete;  ///< Deleted copy constructor.
  Solver(Solver&& root) = delete;       ///< Deleted move constructor.
  Solver& operator=(const Solver&) =
      delete;                            ///< Deleted copy assignment operator.
  Solver& operator=(Solver&&) = delete;  ///< Deleted move assignment operator.
};