#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "phc_traits.hpp"

namespace phc
{
/**
 * @class PHCWrapper
 * @brief A wrapper class for handling polynomial homotopy continuation
 * operations.
 */
class PHCWrapper
{
 public:
  /**
   * @brief Clears the internal state of the wrapper.
   */
  void Clear() const;

  /**
   * @brief Initializes the number of polynomials to be solved.
   * @param n The number of polynomials.
   */
  void InitializeNumberOfPolynomials(size_t n) const;

  /**
   * @brief Inserts a polynomial into the system at the specified index.
   * @param polynomial The polynomial as a string view.
   * @param idx The index where the polynomial is inserted.
   */
  void InsertPolynomial(std::string_view polynomial, size_t idx) const;

  /**
   * @brief Gets the number of symbols used in the polynomials.
   * @return The number of symbols.
   */
  size_t GetAmountOfSymbols() const;

  /**
   * @brief Checks if the system of polynomials is square (same number of
   * equations and variables).
   * @return True if the system is square, false otherwise.
   */
  bool IsSquare() const;

  /**
   * @brief Sets the symbols used in the polynomials.
   * @param symbols A vector of symbol names.
   */
  void SetSymbols(const std::vector<std::string>& symbols);

  /**
   * @struct Root
   * @brief Represents a root of the polynomial system.
   */
  struct Root
  {
    size_t multiplicity;  ///< The multiplicity of the root.

    /**
     * @struct Complex
     * @brief Represents a complex number.
     */
    struct Complex
    {
      std::string real;  ///< The real part of the complex number.
      std::string imag;  ///< The imaginary part of the complex number.

      /**
       * @brief Equality operator for comparing two complex numbers.
       * @param other The other complex number to compare.
       * @return True if equal, false otherwise.
       */
      bool operator==(const Complex&) const = default;
    };

    std::vector<Complex> data;  ///< The data representing the root.

    /**
     * @brief Equality operator for comparing two roots.
     * @param other The other root to compare.
     * @return True if equal, false otherwise.
     */
    bool operator==(const Root&) const = default;
  };

  /**
   * @brief Parses a root from a string representation.
   * @param root_string The string representation of the root.
   * @param max_precision The maximum precision for parsing.
   * @return The parsed root.
   */
  Root ParseRoot(std::string root_string, size_t max_precision = 10) const;

  /**
   * @brief Solves the system of polynomials.
   * @return A vector of roots.
   */
  std::vector<Root> Solve() const;

  /**
   * @brief Gets the total degree of the polynomial system.
   * @return The total degree.
   */
  size_t GetTotalDegree() const;

  /**
   * @brief Gets the Bezout number of the polynomial system.
   * @return The Bezout number.
   */
  size_t GetBezoutNumber() const;

  /**
   * @brief Gets the singleton instance of the PHCWrapper.
   * @return A reference to the PHCWrapper instance.
   */
  static PHCWrapper& GetInstance();

 private:
  /**
   * @brief Constructs a PHCWrapper instance.
   */
  PHCWrapper();
  PHCWrapper(PHCWrapper&&) = delete;
  PHCWrapper(const PHCWrapper&) = delete;
  PHCWrapper& operator=(PHCWrapper&&) = delete;
  PHCWrapper& operator=(const PHCWrapper&) = delete;
  ~PHCWrapper();

  /**
   * @brief Implementation of the PHCWrapper for a specific precision.
   * @tparam P The precision type.
   */
  template <Precision P>
  class PHCWrapperImplementation;
  std::unique_ptr<PHCWrapperImplementation<Precision::Standard>>
      implementation_;  ///< The implementation instance.
};

}  // namespace phc