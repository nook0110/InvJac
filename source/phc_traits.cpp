#include "phc_traits.hpp"

#include <celcon.h>
#include <next_track.h>
#include <phcpack.h>
#include <product.h>
#include <solcon.h>
#include <syscon.h>
#include <witsols.h>

namespace phc
{
namespace base
{
int SystemContainerFunctions::ClearSymbolTable()
{
  return syscon_clear_symbol_table();
}

int SystemContainerFunctions::AddSymbol(int size, const char* data)
{
  return syscon_add_symbol(size, data);
}

int SystemContainerFunctions::NumberOfSymbols(int* n)
{
  return syscon_number_of_symbols(n);
}
}  // namespace base

int SystemContainerTraits<Precision::Standard>::ClearSystem()
{
  return syscon_clear_standard_system();
}

int SystemContainerTraits<Precision::Standard>::NumberOfPolynomials(int* n)
{
  return syscon_number_of_standard_polynomials(n);
}

int SystemContainerTraits<Precision::Standard>::StorePolynomial(
    int number_of_characters, int number_of_variables, int index_of_polynomial,
    const char* polynomial_string)
{
  return syscon_store_standard_polynomial(
      number_of_characters, number_of_variables, index_of_polynomial,
      polynomial_string);
}

int SystemContainerTraits<Precision::Standard>::InitializeNumberOfPolynomials(
    int n)
{
  return syscon_initialize_number_of_standard_polynomials(n);
}

int SolutionContainerTraits<Precision::Standard>::ClearSolutions()
{
  return solcon_clear_standard_solutions();
}

int SolutionContainerTraits<Precision::Standard>::NumberOfSolutions(int* n)
{
  return solcon_number_of_standard_solutions(n);
}

int SolutionContainerTraits<Precision::Standard>::LengthSolutionString(
    int solution_number, int* length)
{
  return solcon_length_standard_solution_string(solution_number, length);
}

int SolutionContainerTraits<Precision::Standard>::WriteSolutionString(
    int solution_number, int solution_string_length, char* solution_string)
{
  return solcon_write_standard_solution_string(
      solution_number, solution_string_length, solution_string);
}

namespace base
{
void PHCPackFunctions::AdaInit() { return adainit(); }
void PHCPackFunctions::AdaFinal() { return adafinal(); }
int PHCPackFunctions::ClearData() { return clear_data(); }
}  // namespace base

int PHCPackTraits<Precision::Standard>::SolveSystem(int* root_count, int silent,
                                                    int* nrcs, char* rocos,
                                                    int nbtasks, int vrb)
{
  return solve_standard_system(root_count, silent, nrcs, rocos, nbtasks, 0,
                               vrb);
}

}  // namespace phc