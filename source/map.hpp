#pragma once

#include <ginac/ginac.h>
#include <ginac/matrix.h>
#include <ginac/power.h>
#include <glog/logging.h>

#include <cassert>
#include <cstddef>
#include <optional>
#include <sstream>
#include <vector>

#include "point.hpp"
#include "symbol.hpp"

using Polynomial = GiNaC::ex;

inline std::string ToStr(const Polynomial& poly)
{
  std::stringstream ss;
  ss << poly;
  return ss.str();
}

class Map
{
 public:
  using PolyMatrix = GiNaC::matrix;

  explicit Map(std::vector<Polynomial> polynomials)
      : polynomials_(std::move(polynomials))
  {}

  Point Image(const Point& point) const;

  PolyMatrix GetJacobianMatrix() const
  {
    PolyMatrix jacobian_mat(static_cast<unsigned int>(GetDimensions()),
                            static_cast<unsigned int>(GetDimensions()));
    for (unsigned int i = 0; i < GetDimensions(); ++i)
    {
      for (unsigned int j = 0; j < GetDimensions(); ++j)
      {
        jacobian_mat(i, j) = polynomials_[i].diff(Symbols::GetSymbol(j));
      }
    }

    return jacobian_mat;
  }

  Polynomial GetJacobian() const { return GetJacobianMatrix().determinant(); }

  bool HasContraction() const;

  size_t GetDimensions() const { return polynomials_.size(); }
  const Polynomial& GetPoly(size_t i) const { return polynomials_[i]; }
  size_t GetExtensionDegree() const
  {
    if (!extension_degree_)
    {
      EvaluateExtensionDegree();
    }
    return extension_degree_.value();
  }

  std::string ToStr() const
  {
    return std::accumulate(std::next(polynomials_.begin()), polynomials_.end(),
                           ::ToStr(polynomials_.front()),
                           [](std::string str, const Polynomial& poly)
                           { return std::move(str) + ", " + ::ToStr(poly); });
  }

  std::vector<Polynomial>::iterator begin() { return polynomials_.begin(); }
  std::vector<Polynomial>::const_iterator begin() const
  {
    return polynomials_.begin();
  }
  std::vector<Polynomial>::iterator end() { return polynomials_.end(); }
  std::vector<Polynomial>::const_iterator end() const
  {
    return polynomials_.end();
  }

 private:
  void EvaluateExtensionDegree() const;
  std::vector<Polynomial> polynomials_;

  mutable std::optional<size_t> extension_degree_ = {};
};

inline Polynomial GenerateMonomial(std::vector<size_t> degrees)
{
  Polynomial result = 1;
  for (size_t symbol = 0; symbol < degrees.size(); ++symbol)
  {
    result *= GiNaC::pow(Symbols::GetSymbol(symbol), degrees[symbol]);
  }
  return result;
}

inline Polynomial GeneratePolynomial(const std::vector<Polynomial>& monomials,
                                     const std::vector<Complex>& coefficients,
                                     size_t idx)
{
  if (coefficients.empty())
  {
    return 0;
  }
  Polynomial result = 0;
  size_t monomial = 0;
  while (idx > 0 && monomial < monomials.size())
  {
    result += monomials[monomial++] * coefficients[idx % coefficients.size()];
    idx /= coefficients.size();
  }

  return result;
}