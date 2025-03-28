#pragma once

namespace phc
{
/**
 * @enum Precision
 * @brief Specifies the precision level for computations.
 */
enum class Precision
{
  Standard  ///< Standard precision.
};

namespace base
{
/**
 * @struct SystemContainerFunctions
 * @brief Provides base functions for managing the system container.
 */
struct SystemContainerFunctions
{
  /**
   * @brief Clears the symbol table.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int ClearSymbolTable();

  /**
   * @brief Adds a symbol to the symbol table.
   * @param size The size of the symbol.
   * @param data The symbol data.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int AddSymbol(int size, const char* data);

  /**
   * @brief Gets the number of symbols in the symbol table.
   * @param n Pointer to store the number of symbols.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int NumberOfSymbols(int* n);
};
}  // namespace base

/**
 * @struct SystemContainerTraits
 * @brief Traits for managing system containers based on precision.
 * @tparam P The precision type.
 */
template <Precision P>
struct SystemContainerTraits : public base::SystemContainerFunctions
{
  /**
   * @brief Clears the system. Deleted for this precision.
   */
  [[nodiscard]] static int ClearSystem() = delete;

  /**
   * @brief Gets the number of polynomials. Deleted for this precision.
   */
  [[nodiscard]] static int NumberOfPolynomials(int* n) = delete;

  /**
   * @brief Stores a polynomial in the system. Deleted for this precision.
   */
  [[nodiscard]] static int StorePolynomial(
      int number_of_characters, int number_of_variables,
      int index_of_polynomial, const char* polynomial_string) = delete;

  /**
   * @brief Initializes the number of polynomials. Deleted for this precision.
   */
  [[nodiscard]] static int InitializeNumberOfPolynomials(int n) = delete;
};

/**
 * @struct SystemContainerTraits<Precision::Standard>
 * @brief Specialization of SystemContainerTraits for standard precision.
 */
template <>
struct SystemContainerTraits<Precision::Standard>
    : public base::SystemContainerFunctions
{
  /**
   * @brief Clears the system.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int ClearSystem();

  /**
   * @brief Gets the number of polynomials.
   * @param n Pointer to store the number of polynomials.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int NumberOfPolynomials(int* n);

  /**
   * @brief Stores a polynomial in the system.
   * @param number_of_characters The number of characters in the polynomial.
   * @param number_of_variables The number of variables in the polynomial.
   * @param index_of_polynomial The index of the polynomial.
   * @param polynomial_string The polynomial string.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int StorePolynomial(int number_of_characters,
                                           int number_of_variables,
                                           int index_of_polynomial,
                                           const char* polynomial_string);

  /**
   * @brief Initializes the number of polynomials.
   * @param n The number of polynomials.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int InitializeNumberOfPolynomials(int n);
};

namespace base
{
/**
 * @struct SolutionContainerFunctions
 * @brief Provides base functions for managing the solution container.
 */
struct SolutionContainerFunctions
{};
}  // namespace base

/**
 * @struct SolutionContainerTraits
 * @brief Traits for managing solution containers based on precision.
 * @tparam P The precision type.
 */
template <Precision P>
struct SolutionContainerTraits : public base::SolutionContainerFunctions
{
  /**
   * @brief Clears the solutions. Deleted for this precision.
   */
  [[nodiscard]] static int ClearSolutions() = delete;

  /**
   * @brief Gets the number of solutions. Deleted for this precision.
   */
  [[nodiscard]] static int NumberOfSolutions(int* n) = delete;

  /**
   * @brief Gets the length of a solution string. Deleted for this precision.
   */
  [[nodiscard]] static int LengthSolutionString(int solution_number,
                                                int* length) = delete;

  /**
   * @brief Writes a solution string. Deleted for this precision.
   */
  [[nodiscard]] static int WriteSolutionString(int solution_number,
                                               int solution_string_length,
                                               char* solution_string) = delete;
};

/**
 * @struct SolutionContainerTraits<Precision::Standard>
 * @brief Specialization of SolutionContainerTraits for standard precision.
 */
template <>
struct SolutionContainerTraits<Precision::Standard>
    : public base::SolutionContainerFunctions
{
  /**
   * @brief Clears the solutions.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int ClearSolutions();

  /**
   * @brief Gets the number of solutions.
   * @param n Pointer to store the number of solutions.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int NumberOfSolutions(int* n);

  /**
   * @brief Gets the length of a solution string.
   * @param solution_number The solution number.
   * @param length Pointer to store the length of the solution string.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int LengthSolutionString(int solution_number,
                                                int* length);

  /**
   * @brief Writes a solution string.
   * @param solution_number The solution number.
   * @param solution_string_length The length of the solution string.
   * @param solution_string Pointer to store the solution string.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int WriteSolutionString(int solution_number,
                                               int solution_string_length,
                                               char* solution_string);
};

namespace base
{
/**
 * @struct PHCPackFunctions
 * @brief Provides base functions for PHCpack operations.
 */
struct PHCPackFunctions
{
  /**
   * @brief Initializes the PHCpack environment.
   */
  static void AdaInit();

  /**
   * @brief Finalizes the PHCpack environment.
   */
  static void AdaFinal();

  /**
   * @brief Clears PHCpack data.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int ClearData();
};
}  // namespace base

/**
 * @struct PHCPackTraits
 * @brief Traits for PHCpack operations based on precision.
 * @tparam P The precision type.
 */
template <Precision P>
struct PHCPackTraits : public base::PHCPackFunctions
{
  /**
   * @brief Solves the system. Deleted for this precision.
   */
  [[nodiscard]] static int SolveSystem(int* root_count, int silent, int* nrcs,
                                       char* rocos, int nbtasks,
                                       int vrb) = delete;
};

/**
 * @struct PHCPackTraits<Precision::Standard>
 * @brief Specialization of PHCPackTraits for standard precision.
 */
template <>
struct PHCPackTraits<Precision::Standard> : public base::PHCPackFunctions
{
  /**
   * @brief Solves the system.
   * @param root_count Pointer to store the number of roots.
   * @param silent Whether to suppress output.
   * @param nrcs Pointer to store additional result data.
   * @param rocos Pointer to store the root data.
   * @param nbtasks Number of tasks to perform.
   * @param vrb Verbosity level.
   * @return Status code of the operation.
   */
  [[nodiscard]] static int SolveSystem(int* root_count, int silent, int* nrcs,
                                       char* rocos, int nbtasks, int vrb);
};

}  // namespace phc