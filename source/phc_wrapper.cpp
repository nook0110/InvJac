#include "phc_wrapper.hpp"

#include <glog/logging.h>
// PHCPack
#include <celcon.h>
#include <next_track.h>
#include <phcpack.h>
#include <product.h>
#include <solcon.h>
#include <syscon.h>
#include <witsols.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

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

  PHCWrapper::Root ParseRoot(std::string solution_string) const
  {
    // clang-format off
    /*
    t :  0.00000000000000E+00   0.00000000000000E+00
    m : 1
    the solution for t :
     x_0 : 9.96676921350590249034125378116507E-03      1.46150526361638135612257536395424E-01    
     x_1 : 1.15374259155635172650000000000000E+00      -1.07774252644852808200000000000001E+00   
    == err :  1.233E-32 = rco :  3.127E-02 = res :  5.065E-32 =
    */
    // clang-format on
    PHCWrapper::Root root;
    root.data.resize(std::ranges::count(solution_string, '\n') - 3);
    std::stringstream ss(std::move(solution_string));

    std::string skip;
    ss >> skip >> skip >> skip >>
        skip;  // 't :  0.00000000000000E+00   0.00000000000000E+00'
    ss >> skip >> skip >> root.multiplicity >> std::ws;  // 'm : 1'

    std::getline(ss, skip);  // skip 'the solution for t :'
    for (auto& complex : root.data)
    {
      ss >> skip >> skip >> complex.real >> complex.imag;
    }

    return root;
  }

  ~PHCWrapperImplementation();
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
  solcon_number_of_standard_solutions(&root_count);

  std::vector<PHCWrapper::Root> ans;
  ans.reserve(root_count);
  for (size_t i = 0; i < root_count; ++i)
  {
    int solution_number = static_cast<int>(i + 1);
    PHCWrapper::Root root;
    std::string solution_string;
    int n;
    solcon_length_standard_solution_string(solution_number, &n);
    solution_string.resize(n);
    if (solcon_write_standard_solution_string(solution_number, n,
                                              solution_string.data()))
    {
      LOG(DFATAL) << "Failed to retrieve solution for index: " << i;
    }

    ans.emplace_back(ParseRoot(solution_string));
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

PHCWrapper::Root PHCWrapper::ParseRoot(std::string root_string) const
{
  return implementation_->ParseRoot(std::move(root_string));
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

}  // namespace phc