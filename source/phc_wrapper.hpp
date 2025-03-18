#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "phc_traits.hpp"

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
    struct Complex
    {
      std::string real;
      std::string imag;
      bool operator==(const Complex&) const = default;
    };
    std::vector<Complex> data;

    bool operator==(const Root&) const = default;
  };
  Root ParseRoot(std::string root_string, size_t max_precision = 10) const;
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

  template <Precision P>
  class PHCWrapperImplementation;
  std::unique_ptr<PHCWrapperImplementation<Precision::Standard>>
      implementation_;
};

}  // namespace phc