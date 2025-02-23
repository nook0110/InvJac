#include "phc_wrapper.hpp"

#include <glog/logging.h>
// PHCPack
#include <cln/cln.h>
#include <ginac/ginac.h>
#include <phc/celcon.h>
#include <phc/phcpack.h>
#include <phc/solcon.h>
#include <phc/syscon.h>

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include "map.hpp"
#include "phc/next_track.h"
#include "phc/product.h"
#include "phc/witsols.h"
#include "point.hpp"

namespace phc
{
class PHCWrapper::PHCWrapperImplementation
{
 public:
  PHCWrapperImplementation();
  PHCWrapperImplementation(PHCWrapperImplementation&&) = delete;
  PHCWrapperImplementation(const PHCWrapperImplementation&) = delete;
  PHCWrapperImplementation& operator=(PHCWrapperImplementation&&) = delete;
  PHCWrapperImplementation& operator=(const PHCWrapperImplementation&) = delete;
  void Clear() const;
  void InitializeNumberOfPolynomials(size_t n) const;
  void SetSymbols(const std::vector<std::string>& symbols) const
  {
    syscon_clear_symbol_table();
    for (const auto& symbol : symbols)
    {
      syscon_add_symbol(static_cast<int>(symbol.size()), symbol.data());
    }
  }

  size_t GetAmountOfSymbols() const;
  size_t GetAmountOfEquations() const;
  bool IsSquare() const;

  void InsertPolynomial(std::string_view polynomial, size_t idx) const;

  std::vector<PHCWrapper::Root> Solve() const;

  ~PHCWrapperImplementation();

 private:
  std::string system_;
};

PHCWrapper::PHCWrapperImplementation::PHCWrapperImplementation()
{
  adainit();
  VLOG(0) << "PHCWrapper initialized.";
}

void PHCWrapper::PHCWrapperImplementation::Clear() const
{
  syscon_clear_standard_system();
  clear_data();
  clear_homotopy();
  clear_set_structure();
  clear_standard_tracker();
  solcon_clear_standard_solutions();
  syscon_clear_symbol_table();
  clear_standard_witsols();
}

void PHCWrapper::PHCWrapperImplementation::InitializeNumberOfPolynomials(
    size_t n) const
{
  Clear();
  if (syscon_initialize_number_of_standard_polynomials(static_cast<int>(n)))
  {
    LOG(DFATAL) << "Failed to initialize number of standard polynomials.";
  }
  VLOG(0) << "Initialized number of standard polynomials: " << n;
}

size_t PHCWrapper::PHCWrapperImplementation::GetAmountOfSymbols() const
{
  int n;
  if (syscon_number_of_symbols(&n))
  {
    LOG(DFATAL) << "Failed to retrieve number of symbols.";
    return 0;
  }
  return n;
}

size_t PHCWrapper::PHCWrapperImplementation::GetAmountOfEquations() const
{
  int n;
  if (syscon_number_of_standard_polynomials(&n))
  {
    LOG(DFATAL) << "Failed to retrieve number of equations.";
    return 0;
  }
  return n;
}

bool PHCWrapper::PHCWrapperImplementation::IsSquare() const
{
  return GetAmountOfSymbols() == GetAmountOfEquations();
}

void PHCWrapper::PHCWrapperImplementation::InsertPolynomial(
    std::string_view polynomial, size_t idx) const
{
  DLOG_IF(FATAL, polynomial.back() != ';')
      << "Polynomial should end with ';', you tried to insert: " << polynomial;

  if (syscon_store_standard_polynomial(static_cast<int>(polynomial.size()),
                                       static_cast<int>(GetAmountOfEquations()),
                                       static_cast<int>(idx + 1),
                                       polynomial.data()))
  {
    LOG(DFATAL) << "Failed to store standard polynomial for equation: "
                << polynomial;
  }
  VLOG(0) << "Inserted polynomial at index " << idx << ": " << polynomial;
}

std::vector<PHCWrapper::Root> PHCWrapper::PHCWrapperImplementation::Solve()
    const
{
  DLOG_IF(FATAL, !IsSquare()) << "System is not square!";
  size_t length = GetAmountOfEquations();

  int root_count;
  if (solve_standard_system(&root_count, 1, nullptr, nullptr, 1, 0, 0))
  {
    LOG(DFATAL) << "Failed to solve system";
  }

  std::vector<PHCWrapper::Root> ans;
  ans.reserve(root_count);
  for (size_t i = 0; i < root_count; ++i)
  {
    std::vector<double> solution(length * 2 + 5);
    int multiplicity;
    if (solcon_retrieve_standard_solution(static_cast<int>(length),
                                          static_cast<int>(i + 1),
                                          &multiplicity, solution.data()))
    {
      // LOG(DFATAL) << "Failed to retrieve solution for index: " << i;
    }

    ans.emplace_back(static_cast<size_t>(multiplicity), std::move(solution));
  }

  VLOG(1) << "Solved system with " << root_count << " solutions.";
  return ans;
}

PHCWrapper::PHCWrapperImplementation::~PHCWrapperImplementation()
{
  adafinal();
  VLOG(0) << "PHCWrapper finalized.";
}

PHCWrapper::PHCWrapper()
    : implementation_(std::make_unique<PHCWrapperImplementation>())
{}

void PHCWrapper::Clear() const { implementation_->Clear(); }

void PHCWrapper::InitializeNumberOfPolynomials(size_t n) const
{
  implementation_->InitializeNumberOfPolynomials(n);
}

void PHCWrapper::InsertPolynomial(std::string_view polynomial, size_t idx) const
{
  implementation_->InsertPolynomial(polynomial, idx);
}

size_t PHCWrapper::GetAmountOfSymbols() const
{
  return implementation_->GetAmountOfSymbols();
}

bool PHCWrapper::IsSquare() const { return implementation_->IsSquare(); }

void PHCWrapper::SetSymbols(const std::vector<std::string>& symbols)
{
  implementation_->SetSymbols(symbols);
}

std::vector<PHCWrapper::Root> PHCWrapper::Solve() const
{
  return implementation_->Solve();
}

PHCWrapper& PHCWrapper::GetInstance()
{
  static PHCWrapper instance;
  return instance;
}

PHCWrapper::~PHCWrapper() = default;

bool Equation::IsSquare() const
{
  PHCWrapper::GetInstance().InitializeNumberOfPolynomials(polynomials_.size());
  LoadEquations();
  return PHCWrapper::GetInstance().IsSquare();
}

Equation::Equation(size_t amount_of_equations)
{
  ReserveAmountOfEquations(amount_of_equations);
}

Equation::Solution Equation::Solve() const
{
  PHCWrapper::GetInstance().InitializeNumberOfPolynomials(polynomials_.size());

  std::vector<std::string> symbols_name;
  auto symbols = Symbols::GetSymbolsList(polynomials_.size());
  std::ranges::transform(symbols, std::back_inserter(symbols_name),
                         [](const auto& symbol) { return ToStr(symbol); });
  PHCWrapper::GetInstance().SetSymbols(symbols_name);

  LoadEquations();

  auto solution_data = PHCWrapper::GetInstance().Solve();
  Solution solution;
  solution.Reserve(solution_data.size());
  for (const auto& [multiplicity, point_data] : solution_data)
  {
    auto dimensions = (point_data.size() - 5) / 2;
    std::vector<Complex> coords;
    coords.reserve(dimensions);
    for (size_t i = 0; i < dimensions; ++i)
    {
      auto real = point_data[i * 2 + 2];
      auto im = point_data[i * 2 + 3];
      coords.emplace_back(
          cln::complex(cln::rationalize(real), cln::rationalize(im)));
    }
    solution.Append(multiplicity, Point(std::move(coords)));
  }

  return solution;
}

void Equation::ReserveAmountOfEquations(size_t n) { polynomials_.reserve(n); }

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
    PHCWrapper::GetInstance().InsertPolynomial(polynomials_[i], i);
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

}  // namespace phc