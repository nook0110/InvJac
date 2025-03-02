#pragma once

#include "map.hpp"
#include "point.hpp"

/**
 * @brief Class representing an equation.
 */
class Equation
{
 public:
  /**
   * @brief Struct representing a root of the equation.
   */
  struct Root
  {
    size_t multiplicity;  ///< Multiplicity of the root.
    Point point;          ///< Point representing the root.
  };

  /**
   * @brief Class representing the solution of the equation.
   */
  class Solution
  {
   public:
    /**
     * @brief Gets the count of roots.
     * @tparam multiplicity Flag to include multiplicity in the count.
     * @return Number of roots.
     */
    template <bool multiplicity = true>
    size_t GetRootCount() const;

    std::vector<Root>::iterator begin() { return roots_.begin(); }
    std::vector<Root>::iterator end() { return roots_.end(); }

    std::vector<Root>::const_iterator begin() const { return roots_.cbegin(); }
    std::vector<Root>::const_iterator end() const { return roots_.cend(); }

   private:
    friend Equation;
    Solution() = default;

    /**
     * @brief Appends a root to the solution.
     * @param multiplicity Multiplicity of the root.
     * @param point Point representing the root.
     */
    void Append(size_t multiplicity, Point point)
    {
      roots_.emplace_back(multiplicity, std::move(point));
    }

    /**
     * @brief Reserves space for a number of roots.
     * @param n Number of roots to reserve space for.
     */
    void Reserve(size_t n) { roots_.reserve(n); }

    std::vector<Root> roots_;  ///< Vector of roots.
  };

  /**
   * @brief Constructs an Equation.
   * @param amount_of_equations Number of equations.
   */
  explicit Equation(size_t amount_of_equations = 0);

  /**
   * @brief Checks if the equation system is square.
   * @return True if the system is square, false otherwise.
   */
  bool IsSquare() const;

  /**
   * @brief Solves the equation.
   * @return Solution of the equation.
   */
  Solution Solve() const;

  /**
   * @brief Reserves space for a number of equations.
   * @param n Number of equations to reserve space for.
   */
  void ReserveAmountOfEquations(size_t n);

  /**
   * @brief Appends an equation to the system.
   * @param poly Polynomial representing the equation.
   */
  void AppendEquation(const Polynomial& poly);

 private:
  /**
   * @brief Appends an equation to the system.
   * @param equation String representing the equation.
   */
  void AppendEquation(std::string equation);

  /**
   * @brief Loads the equations.
   */
  void LoadEquations() const;

  std::vector<std::string> polynomials_;  ///< Vector of polynomial equations.
};

template <>
size_t Equation::Solution::GetRootCount<true>() const;
template <>
size_t Equation::Solution::GetRootCount<false>() const;