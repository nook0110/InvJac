#include "equation.hpp"

#include <cln/cln.h>
#include <ginac/ginac.h>

#include <vector>

#include "phc_wrapper.hpp"
#include "source/point.hpp"

bool Equation::IsSquare() const
{
  phc::PHCWrapper::GetInstance().InitializeNumberOfPolynomials(
      polynomials_.size());
  LoadEquations();
  return phc::PHCWrapper::GetInstance().IsSquare();
}

Equation::Equation(size_t amount_of_equations)
{
  ReserveAmountOfEquations(amount_of_equations);
}

Equation::Solution Equation::Solve() const
{
  phc::PHCWrapper::GetInstance().InitializeNumberOfPolynomials(
      polynomials_.size());

  std::vector<std::string> symbols_name;
  auto symbols = Symbols::GetSymbolsList(polynomials_.size());
  std::ranges::transform(symbols, std::back_inserter(symbols_name),
                         [](const auto& symbol) { return ToStr(symbol); });
  phc::PHCWrapper::GetInstance().SetSymbols(symbols_name);

  LoadEquations();

  auto solution_data = phc::PHCWrapper::GetInstance().Solve();
  Solution solution;
  solution.Reserve(solution_data.size());
  for (const auto& [multiplicity, point_data] : solution_data)
  {
    std::vector<Complex> coords;
    coords.reserve(point_data.size());
    for (size_t i = 0; i < point_data.size(); ++i)
    {
      coords.emplace_back(
          cln::complex(cln::rationalize(point_data[i].real.data()),
                       cln::rationalize(point_data[i].imag.data())));
    }
    solution.Append(multiplicity, Point(std::move(coords)));
  }

  return solution;
}

void Equation::ReserveAmountOfEquations(size_t n) { polynomials_.reserve(n); }

void Equation::AppendEquation(const Polynomial& poly)
{
  AppendEquation(ToStr(poly));
}

void Equation::AppendEquation(std::string equation)
{
  if (equation.back() != ';')
  {
    equation += ';';
  }
  polynomials_.emplace_back(std::move(equation));
}

void Equation::LoadEquations() const
{
  for (size_t i = 0; i < polynomials_.size(); ++i)
  {
    phc::PHCWrapper::GetInstance().InsertPolynomial(polynomials_[i], i);
  }
}

template <>
size_t Equation::Solution::GetRootCount<true>() const
{
  return std::ranges::fold_left(roots_, size_t{0},
                                [](size_t root_count, const auto& root)
                                { return root_count + root.multiplicity; });
}

template <>
size_t Equation::Solution::GetRootCount<false>() const
{
  return roots_.size();
}
