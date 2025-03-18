#include "phc_wrapper.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "phc_traits.hpp"

namespace phc
{
template <Precision P>
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
    if (SystemContainerTraits<P>::ClearSymbolTable())
    {
      LOG(DFATAL) << "Failed to clear symbol table!";
    }
    for (const auto& symbol : symbols)
    {
      if (SystemContainerTraits<P>::AddSymbol(static_cast<int>(symbol.size()),
                                              symbol.data()))
      {
        LOG(DFATAL) << "Failed to add a symbol: " << symbol;
      }
    }
  }

  size_t GetAmountOfSymbols() const;
  size_t GetAmountOfEquations() const;
  bool IsSquare() const;

  void InsertPolynomial(std::string_view polynomial, size_t idx) const;

  std::vector<PHCWrapper::Root> Solve() const;

  PHCWrapper::Root ParseRoot(std::string solution_string,
                             size_t max_precision = 10) const
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

    auto strip_number = [max_precision](std::string_view str)
    {
      size_t e_pos = str.find_first_of("Ee");

      if (e_pos == std::string_view::npos)
      {
        return std::string(str);
      }

      std::string_view mantissa = str.substr(0, e_pos);
      size_t dot_pos = mantissa.find('.');
      if (dot_pos != std::string_view::npos)
      {
        size_t end_pos = std::min(dot_pos + max_precision, mantissa.size());
        mantissa = mantissa.substr(0, end_pos);
      }
      return std::string(mantissa) + std::string(str.substr(e_pos));
    };
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
      complex.real = strip_number(complex.real);
      complex.imag = strip_number(complex.imag);
    }

    return root;
  }

  ~PHCWrapperImplementation();
};

template <Precision P>
PHCWrapper::PHCWrapperImplementation<P>::PHCWrapperImplementation()
{
  PHCPackTraits<P>::AdaInit();
  VLOG(0) << "PHCWrapper initialized.";
}

template <Precision P>
void PHCWrapper::PHCWrapperImplementation<P>::Clear() const
{
  if (PHCPackTraits<P>::ClearData())
  {
    LOG(DFATAL) << "Error while clearing data!";
  }
  if (SystemContainerTraits<P>::ClearSymbolTable())
  {
    LOG(DFATAL) << "Error while clearing symbol table!";
  }
  if (SolutionContainerTraits<P>::ClearSolutions())
  {
    LOG(DFATAL) << "Error while clearing solutions!";
  }
}

template <Precision P>
void PHCWrapper::PHCWrapperImplementation<P>::InitializeNumberOfPolynomials(
    size_t n) const
{
  Clear();
  if (SystemContainerTraits<P>::InitializeNumberOfPolynomials(
          static_cast<int>(n)))
  {
    LOG(DFATAL) << "Failed to initialize number of standard polynomials.";
  }
  VLOG(0) << "Initialized number of standard polynomials: " << n;
}

template <Precision P>
size_t PHCWrapper::PHCWrapperImplementation<P>::GetAmountOfSymbols() const
{
  int n;
  if (SystemContainerTraits<P>::NumberOfSymbols(&n))
  {
    LOG(DFATAL) << "Failed to retrieve number of symbols.";
    return 0;
  }
  return n;
}

template <Precision P>
size_t PHCWrapper::PHCWrapperImplementation<P>::GetAmountOfEquations() const
{
  int n;
  if (SystemContainerTraits<P>::NumberOfPolynomials(&n))
  {
    LOG(DFATAL) << "Failed to retrieve number of equations.";
    return 0;
  }
  return n;
}

template <Precision P>
bool PHCWrapper::PHCWrapperImplementation<P>::IsSquare() const
{
  return GetAmountOfSymbols() == GetAmountOfEquations();
}

template <Precision P>
void PHCWrapper::PHCWrapperImplementation<P>::InsertPolynomial(
    std::string_view polynomial, size_t idx) const
{
  DLOG_IF(FATAL, polynomial.back() != ';')
      << "Polynomial should end with ';', you tried to insert: " << polynomial;

  if (SystemContainerTraits<P>::StorePolynomial(
          static_cast<int>(polynomial.size()),
          static_cast<int>(GetAmountOfEquations()), static_cast<int>(idx + 1),
          polynomial.data()))
  {
    LOG(DFATAL) << "Failed to store standard polynomial for equation: "
                << polynomial;
  }
  VLOG(0) << "Inserted polynomial at index " << idx << ": " << polynomial;
}

template <Precision P>
std::vector<PHCWrapper::Root> PHCWrapper::PHCWrapperImplementation<P>::Solve()
    const
{
  DLOG_IF(FATAL, !IsSquare()) << "System is not square!";
  size_t length = GetAmountOfEquations();

  int root_count;
  if (PHCPackTraits<P>::SolveSystem(&root_count, 1, nullptr, nullptr, 1, 0))
  {
    LOG(DFATAL) << "Failed to solve system";
  }
  if (SolutionContainerTraits<P>::NumberOfSolutions(&root_count))
  {
    LOG(DFATAL) << "Failed to get number of solutions";
  }

  std::vector<PHCWrapper::Root> ans;
  ans.reserve(root_count);
  for (size_t i = 0; i < root_count; ++i)
  {
    int solution_number = static_cast<int>(i + 1);
    PHCWrapper::Root root;
    std::string solution_string;
    int n;
    if (SolutionContainerTraits<P>::LengthSolutionString(solution_number, &n))
    {
      LOG(DFATAL) << "Failed to retrieve length of solution string for index: "
                  << i;
    }
    solution_string.resize(n);
    if (SolutionContainerTraits<P>::WriteSolutionString(solution_number, n,
                                                        solution_string.data()))
    {
      LOG(DFATAL) << "Failed to retrieve solution for index: " << i;
    }

    ans.emplace_back(ParseRoot(solution_string));
  }

  VLOG(1) << "Solved system with " << root_count << " solutions.";
  return ans;
}

template <Precision P>
PHCWrapper::PHCWrapperImplementation<P>::~PHCWrapperImplementation()
{
  PHCPackTraits<P>::AdaFinal();
  VLOG(0) << "PHCWrapper finalized.";
}

PHCWrapper::PHCWrapper()
    : implementation_(
          std::make_unique<PHCWrapperImplementation<Precision::Standard>>())
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

PHCWrapper::Root PHCWrapper::ParseRoot(std::string root_string,
                                       size_t max_precision) const
{
  return implementation_->ParseRoot(std::move(root_string), max_precision);
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