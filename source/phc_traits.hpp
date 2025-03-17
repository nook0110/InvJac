#pragma once

namespace phc
{
enum class Precision
{
  Standard
};

namespace base
{
struct SystemContainerFunctions
{
  static int ClearSymbolTable();
  static int AddSymbol(int size, const char* data);
  static int NumberOfSymbols(int* n);
};
}  // namespace base

template <Precision P>
struct SystemContainerTraits : public base::SystemContainerFunctions
{
  static int ClearSystem() = delete;
  static int NumberOfPolynomials(int* n) = delete;
  static int StorePolynomial(int number_of_characters, int number_of_variables,
                             int index_of_polynomial,
                             const char* polynomial_string) = delete;
  static int InitializeNumberOfPolynomials(int n) = delete;
};

template <>
struct SystemContainerTraits<Precision::Standard>
    : public base::SystemContainerFunctions
{
  static int ClearSystem();
  static int NumberOfPolynomials(int* n);
  static int StorePolynomial(int number_of_characters, int number_of_variables,
                             int index_of_polynomial,
                             const char* polynomial_string);
  static int InitializeNumberOfPolynomials(int n);
};

namespace base
{
struct SolutionContainerFunctions
{};
}  // namespace base

template <Precision P>
struct SolutionContainerTraits : public base ::SolutionContainerFunctions
{
  static int ClearSolutions() = delete;
  static int NumberOfSolutions(int* n) = delete;
  static int LengthSolutionString(int solution_number, int* length) = delete;
  static int WriteSolutionString(int solution_number,
                                 int solution_string_length,
                                 char* solution_string) = delete;
};

template <>
struct SolutionContainerTraits<Precision::Standard>
    : public base::SolutionContainerFunctions
{
  static int ClearSolutions();
  static int NumberOfSolutions(int* n);
  static int LengthSolutionString(int solution_number, int* length);
  static int WriteSolutionString(int solution_number,
                                 int solution_string_length,
                                 char* solution_string);
};

namespace base
{
struct PHCPackFunctions
{
  static void AdaInit();
  static void AdaFinal();
  static int ClearData();
};
}  // namespace base

template <Precision P>
struct PHCPackTraits : public base::PHCPackFunctions
{
  static int SolveSystem(int* root_count, int silent, int* nrcs, char* rocos,
                         int nbtasks, int vrb) = delete;
};

template <>
struct PHCPackTraits<Precision::Standard> : public base::PHCPackFunctions
{
  static int SolveSystem(int* root_count, int silent, int* nrcs, char* rocos,
                         int nbtasks, int vrb);
};

}  // namespace phc