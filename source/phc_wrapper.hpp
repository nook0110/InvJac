#pragma once

#include <complex>
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include "point.hpp"
#include "map.hpp"

namespace phc
{
class PHCWrapper
{
 public:
  void Clear() const;
  void InitializeNumberOfPolynomials(size_t n) const;
  void InsertPolynomial(std::string_view polynomial, size_t idx) const;
  size_t GetAmountOfSymbols() const;
  bool IsSquare() const;
  void SetSymbols(const std::vector<std::string>& symbols);

  struct Root
  {
    size_t multiplicity;
    std::vector<double> data;
  };
  std::vector<Root> Solve() const;
  size_t GetTotalDegree() const;
  size_t GetBezoutNumber() const;

  static PHCWrapper& GetInstance();

 private:
  PHCWrapper();
  PHCWrapper(PHCWrapper&&) = delete;
  PHCWrapper(const PHCWrapper&) = delete;
  PHCWrapper& operator=(PHCWrapper&&) = delete;
  PHCWrapper& operator=(const PHCWrapper&) = delete;
  ~PHCWrapper();

  class PHCWrapperImplementation;
  std::unique_ptr<PHCWrapperImplementation> implementation_;
};

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
  void AppendEquation(const Polynomial& poly) { AppendEquation(ToStr(poly)); }

 private:
  void AppendEquation(std::string equation);
  void LoadEquations() const;

  std::vector<std::string> polynomials_;
};

template <>
size_t Equation::Solution::GetRootCount<true>() const;
template <>
size_t Equation::Solution::GetRootCount<false>() const;

}  // namespace phc