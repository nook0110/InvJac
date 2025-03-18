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
  [[nodiscard]] static int ClearSymbolTable();
  [[nodiscard]] static int AddSymbol(int size, const char* data);
  [[nodiscard]] static int NumberOfSymbols(int* n);
};
}  // namespace base

template <Precision P>
struct SystemContainerTraits : public base::SystemContainerFunctions
{
  [[nodiscard]] static int ClearSystem() = delete;
  [[nodiscard]] static int NumberOfPolynomials(int* n) = delete;
  [[nodiscard]] static int StorePolynomial(
      int number_of_characters, int number_of_variables,
      int index_of_polynomial, const char* polynomial_string) = delete;
  [[nodiscard]] static int InitializeNumberOfPolynomials(int n) = delete;
};

template <>
struct SystemContainerTraits<Precision::Standard>
    : public base::SystemContainerFunctions
{
  [[nodiscard]] static int ClearSystem();
  [[nodiscard]] static int NumberOfPolynomials(int* n);
  [[nodiscard]] static int StorePolynomial(int number_of_characters,
                                           int number_of_variables,
                                           int index_of_polynomial,
                                           const char* polynomial_string);
  [[nodiscard]] static int InitializeNumberOfPolynomials(int n);
};

namespace base
{
struct SolutionContainerFunctions
{};
}  // namespace base

template <Precision P>
struct SolutionContainerTraits : public base ::SolutionContainerFunctions
{
  [[nodiscard]] static int ClearSolutions() = delete;
  [[nodiscard]] static int NumberOfSolutions(int* n) = delete;
  [[nodiscard]] static int LengthSolutionString(int solution_number,
                                                int* length) = delete;
  [[nodiscard]] static int WriteSolutionString(int solution_number,
                                               int solution_string_length,
                                               char* solution_string) = delete;
};

template <>
struct SolutionContainerTraits<Precision::Standard>
    : public base::SolutionContainerFunctions
{
  [[nodiscard]] static int ClearSolutions();
  [[nodiscard]] static int NumberOfSolutions(int* n);
  [[nodiscard]] static int LengthSolutionString(int solution_number,
                                                int* length);
  [[nodiscard]] static int WriteSolutionString(int solution_number,
                                               int solution_string_length,
                                               char* solution_string);
};

namespace base
{
struct PHCPackFunctions
{
  static void AdaInit();
  static void AdaFinal();
  [[nodiscard]] static int ClearData();
};
}  // namespace base

template <Precision P>
struct PHCPackTraits : public base::PHCPackFunctions
{
  [[nodiscard]] static int SolveSystem(int* root_count, int silent, int* nrcs,
                                       char* rocos, int nbtasks,
                                       int vrb) = delete;
};

template <>
struct PHCPackTraits<Precision::Standard> : public base::PHCPackFunctions
{
  [[nodiscard]] static int SolveSystem(int* root_count, int silent, int* nrcs,
                                       char* rocos, int nbtasks, int vrb);
};

}  // namespace phc