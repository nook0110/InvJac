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

/**
 * @brief Class representing a mathematical map.
 */
class Map
{
 public:
  using PolyMatrix = GiNaC::matrix;

  /**
   * @brief Constructs a Map with the given polynomials.
   * @param polynomials Vector of polynomials representing the map.
   */
  explicit Map(std::vector<Polynomial> polynomials = {})
      : polynomials_(std::move(polynomials))
  {}

  /**
   * @brief Applies the map to a point.
   * @param point Point to apply the map to.
   * @return New point after applying the map.
   */
  Point Image(const Point& point) const;

  /**
   * @brief Gets the Jacobian matrix of the map.
   * @return Matrix representing the Jacobian.
   */
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

  /**
   * @brief Gets the Jacobian of the map.
   * @return Polynomial representing the Jacobian.
   */
  Polynomial GetJacobian() const { return GetJacobianMatrix().determinant(); }

  /**
   * @brief Checks if the map has a contraction.
   * @return Optional point where the contraction occurs.
   */
  std::optional<Point> HasContraction() const;

  /**
   * @brief Gets the dimensions of the map.
   * @return Number of dimensions.
   */
  size_t GetDimensions() const { return polynomials_.size(); }

  /**
   * @brief Gets the polynomial at the specified index.
   * @param i Index of the polynomial.
   * @return Polynomial at the specified index.
   */
  const Polynomial& GetPoly(size_t i) const { return polynomials_[i]; }

  /**
   * @brief Gets the extension degree of the map.
   * @return Extension degree.
   */
  size_t GetExtensionDegree() const
  {
    if (!extension_degree_)
    {
      EvaluateExtensionDegree();
    }
    return extension_degree_.value();
  }

  /**
   * @brief Converts the map to a string representation.
   * @return String representation of the map.
   */
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
  /**
   * @brief Evaluates the extension degree of the map.
   */
  void EvaluateExtensionDegree() const;

  std::vector<Polynomial> polynomials_;  ///< Polynomials representing the map.
  mutable std::optional<size_t> extension_degree_ =
      {};  ///< Extension degree of the map.
};

/**
 * @brief Generates a monomial with the given degrees.
 * @param degrees Vector of degrees for each symbol.
 * @return Generated monomial.
 */
inline Polynomial GenerateMonomial(std::vector<size_t> degrees)
{
  Polynomial result = 1;
  for (size_t symbol = 0; symbol < degrees.size(); ++symbol)
  {
    result *= GiNaC::pow(Symbols::GetSymbol(symbol), degrees[symbol]);
  }
  return result;
}

/**
 * @brief Generates a polynomial with the given monomials and coefficients.
 * @param monomials Vector of monomials.
 * @param coefficients Vector of coefficients.
 * @param idx Index for generating the polynomial.
 * @return Generated polynomial.
 */
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