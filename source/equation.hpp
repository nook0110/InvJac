#pragma once

#include "map.hpp"
#include "point.hpp"

class Equation
{
 public:
  struct Root
  {
    size_t multiplicity;
    Point point;
  };

  class Solution
  {
   public:
    template <bool multiplicity = true>
    size_t GetRootCount() const;

    std::vector<Root>::iterator begin() { return roots_.begin(); }
    std::vector<Root>::iterator end() { return roots_.end(); }

    std::vector<Root>::const_iterator begin() const { return roots_.cbegin(); }
    std::vector<Root>::const_iterator end() const { return roots_.cend(); }

   private:
    friend Equation;
    Solution() = default;
    void Append(size_t multiplicity, Point point)
    {
      roots_.emplace_back(multiplicity, std::move(point));
    }

    void Reserve(size_t n) { roots_.reserve(n); }

    std::vector<Root> roots_;
  };

  explicit Equation(size_t amount_of_equations = 0);

  bool IsSquare() const;

  Solution Solve() const;

  void ReserveAmountOfEquations(size_t n);
  void AppendEquation(const Polynomial& poly);

 private:
  void AppendEquation(std::string equation);
  void LoadEquations() const;

  std::vector<std::string> polynomials_;
};

template <>
size_t Equation::Solution::GetRootCount<true>() const;
template <>
size_t Equation::Solution::GetRootCount<false>() const;