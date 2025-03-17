#pragma once

namespace phc
{
enum class Precision
{
  Standard
};

struct SystemContainerFunctions
{
  static int ClearSymbolTable();
  static int AddSymbol(int size, const char* data);
  static int NumberOfSymbols(int* n);
};

template <Precision P>
struct SystemContainerTraits;

template <>
struct SystemContainerTraits<Precision::Standard> : SystemContainerFunctions
{
  static int ClearSystem();
  static int NumberOfPolynomials(int* n);
  static int StorePolynomial(int number_of_characters, int number_of_variables,
                             int index_of_polynomial,
                             const char* polynomial_string);
};

struct SolutionContainerFunctions
{};

template <Precision P>
struct SolutionContainerTraits;

template <>
struct SolutionContainerTraits<Precision::Standard> : SolutionContainerFunctions
{
  static int ClearSolutions();
  static int NumberOfSolutions(int* n);
  static int LengthSolutionString(int solution_number, int* length);
};

struct PHCPackFunctions
{
  static int ClearData();
};

template <Precision P>
struct PHCPackTraits;

template <>
struct PHCPackTraits<Precision::Standard> : PHCPackFunctions
{
  static int SolveSystem(int* root_count, int silent, int* nrcs, char* rocos,
                         int nbtasks, int vrb);
};

}  // namespace phc