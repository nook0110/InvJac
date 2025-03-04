#pragma once

#include <ginac/ex.h>
#include <ginac/numeric.h>
#include <glog/logging.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <variant>
#include <vector>

#include "equation.hpp"
#include "map.hpp"
#include "point.hpp"
#include "solver.hpp"
#include "symbol.hpp"

/**
 * @brief Settings for the Checker class.
 */
struct CheckerSettings
{
  size_t iterations = 10;         ///< Number of iterations to perform.
  double epsilon = 1e-4;          ///< Tolerance for the check.
  bool check_contraction = true;  ///< Flag to check for contraction.
};

/**
 * @brief Result of a check operation.
 */
class CheckResult
{
 public:
  /**
   * @brief Represents an error during the check.
   */
  struct Error
  {
    std::string message;  ///< Error message.

    /**
     * @brief Converts the error to a string.
     * @return String representation of the error.
     */
    std::string ToStr() const { return "Error. " + message; }
  };

  /**
   * @brief Represents a failed check.
   */
  struct Failed
  {
    Failed(Point p, Complex v) : point(std::move(p)), value(std::move(v)) {}

    Failed(Failed&&) noexcept = default;
    Failed& operator=(Failed&&) noexcept = default;

    /**
     * @brief Converts the failure to a string.
     * @return String representation of the failure.
     */
    std::string ToStr() const
    {
      return "Failed. Point: " + point.ToStr() + ", Value: " + ::ToStr(value);
    }

    Point point;    ///< Point where the check failed.
    Complex value;  ///< Value at the failed point.
  };

  /**
   * @brief Represents a passed check.
   */
  struct Passed
  {
    /**
     * @brief Converts the success to a string.
     * @return String representation of the success.
     */
    std::string ToStr() const
    {
      return "Passed. Total checks: " + std::to_string(check_amount);
    }
    size_t check_amount;  ///< Number of checks performed.
  };

  /**
   * @brief Represents a contraction found during the check.
   */
  struct Contraction
  {
    /**
     * @brief Converts the contraction to a string.
     * @return String representation of the contraction.
     */
    std::string ToStr() const
    {
      return "Contraction. At point: " + point.ToStr();
    }

    Point point;  ///< Point where the contraction was found.
  };

  /**
   * @brief Represents a Keller result.
   */
  struct Keller
  {
    /**
     * @brief Converts the Keller result to a string.
     * @return String representation of the Keller result.
     */
    std::string ToStr() const
    {
      return "Keller. Jacobian: " + ::ToStr(jacobian);
    }

    Complex jacobian;  ///< Jacobian value.
  };

  using Result = std::variant<Error, Failed, Passed, Contraction, Keller>;

  /**
   * @brief Constructs a CheckResult.
   * @param m Map used for the check.
   * @param res Result of the check.
   */
  CheckResult(Map m, Result res) : map(std::move(m)), result(std::move(res)) {}

  /**
   * @brief Gets the map used for the check.
   * @return Reference to the map.
   */
  const Map& GetMap() const { return map; }

  /**
   * @brief Gets the result of the check.
   * @return Reference to the result.
   */
  const Result& GetResult() const { return result; }

  /**
   * @brief Converts the check result to a string.
   * @return String representation of the check result.
   */
  std::string ToStr() const
  {
    return "Map: " + map.ToStr() + ". Result: " +
           std::visit([](auto&& arg) { return arg.ToStr(); }, result);
  }

 private:
  Map map;        ///< Map used for the check.
  Result result;  ///< Result of the check.
};

/**
 * @brief Generates a point with a unit Jacobian.
 * @param map Map used for the generation.
 * @param point Initial point.
 * @return Optional point with a unit Jacobian.
 */
std::optional<Point> GeneratePointWithSpecificJacobian(const Map& map,
                                                       const Point& point,
                                                       const Complex& value);

/**
 * @brief Class to perform checks on a map.
 */
class Checker
{
 public:
  /**
   * @brief Constructs a Checker with the given settings.
   * @param settings Settings for the checker.
   */
  explicit Checker(const CheckerSettings& settings = CheckerSettings())
      : settings_(settings), map_(nullptr)
  {}

  /**
   * @brief Sets the map to be checked.
   * @param new_map Pointer to the new map.
   */
  void SetMap(const Map* new_map)
  {
    VLOG(0) << "Setting new map.";
    map_ = new_map;
  }

  /**
   * @brief Performs the check on the set map.
   * @return Result of the check.
   */
  CheckResult::Result PerformCheck()
  {
    LOG_IF(FATAL, !map_) << "Attempting to perform a check without a map set.";
    VLOG(4) << "Starting check with settings: iterations = "
            << settings_.iterations << ", epsilon = " << settings_.epsilon
            << ", check_contraction = " << settings_.check_contraction;

    try
    {
      auto jacobian = map_->GetJacobian();
      if (GiNaC::is_a<Complex>(jacobian))
      {
        VLOG(1) << "Jacobian is a complex number.";
        return CheckResult::Keller{GiNaC::ex_to<Complex>(jacobian)};
      }

      for (size_t iteration = 0; iteration < settings_.iterations; ++iteration)
      {
        VLOG(2) << "Iteration " << iteration + 1 << " of "
                << settings_.iterations;
        static const std::array<Complex, 5> jacobian_values{
            Complex(1), Complex(1, 2), Complex(-1), Complex(-1, 2), Complex(2)};

        TestResult result;
        Point point;
        for (const auto& jacobian_value : jacobian_values)
        {
          VLOG(3) << "Testing with jacobian value: " << jacobian_value;
          point = Point::GenerateRandom(map_->GetDimensions());
          VLOG(3) << "Generated random point: " << point.ToStr();
          point = map_->Image(
              GeneratePointWithSpecificJacobian(*map_, point, jacobian_value)
                  .value_or(point));
          VLOG(3) << "Mapped point: " << point.ToStr();

          result = TestPoint(point);
          if (!std::holds_alternative<SingularPoint>(result))
          {
            break;
          }
        }

        if (std::holds_alternative<SingularPoint>(result))
        {
          VLOG(3) << "All points resulted in singular points, generating a new "
                     "random point.";
          point = Point::GenerateRandom(map_->GetDimensions());
          result = TestPoint(point);
        }

        if (std::holds_alternative<NonZeroJacobian>(result))
        {
          VLOG(2) << "Non-zero Jacobian found.";
          if (map_->GetDimensions() == 2)
          {
            if (auto point = map_->HasContraction())
            {
              VLOG(2) << "Contraction found at point: " << point->ToStr();
              return CheckResult::Contraction{*point};
            }
          }

          VLOG(4) << "Non-zero Jacobian value: "
                  << std::get<NonZeroJacobian>(result).value;
          return CheckResult::Failed{point,
                                     std::get<NonZeroJacobian>(result).value};
        }
      }
    }
    catch (const std::exception& e)
    {
      LOG(ERROR) << "Exception occurred: " << e.what();
      return CheckResult::Error{e.what()};
    }
    VLOG(1) << "Check passed after " << settings_.iterations << " iterations.";
    return CheckResult::Passed{settings_.iterations};
  }

 private:
  /**
   * @brief Tests a point on the map.
   * @param point Point to be tested.
   * @return Optional complex value if the test fails.
   */

  struct NonZeroJacobian
  {
    Complex value;
  };

  struct ZeroJacobian
  {};

  struct SingularPoint
  {};

  using TestResult = std::variant<NonZeroJacobian, ZeroJacobian, SingularPoint>;

  TestResult TestPoint(const Point& point) const
  {
    VLOG(0) << "Testing point: " << point.ToStr();
    auto solutions = Solver::GetInstance().Solve(*map_, point);

    if (solutions.GetRootCount() < map_->GetExtensionDegree())
    {
      return SingularPoint{};
    }

    Complex sum_reciprocals = 0;

    for (const auto& [multiplicity, solution] : solutions)
    {
      VLOG(3) << "Multiplicity: " << multiplicity
              << " Solution: " << solution.ToStr();

      auto jacobian = map_->GetJacobian();
      auto term =
          multiplicity / SubstituteJacobian(std::move(jacobian), solution);
      sum_reciprocals += term;

      VLOG(3) << "Term: " << term.evalf();
    }

    auto approximate_sum = GiNaC::abs(sum_reciprocals).to_double();
    VLOG(1) << "Sum of reciprocals of Jacobians: " << approximate_sum;

    bool result = approximate_sum < settings_.epsilon;
    VLOG(1) << "Test result: " << (result ? "passed" : "failed");
    if (result)
    {
      return ZeroJacobian{};
    }
    return NonZeroJacobian{approximate_sum};
  }

  /**
   * @brief Substitutes the Jacobian with the given point.
   * @param jacobian Jacobian polynomial.
   * @param point Point to substitute.
   * @return Complex value after substitution.
   */
  Complex SubstituteJacobian(Polynomial jacobian, const Point& point) const
  {
    return GiNaC::ex_to<Complex>(jacobian.subs(
        Symbols::GetSymbolsList(map_->GetDimensions()), point.ToLst()));
  }

  const Map* map_ = nullptr;  ///< Pointer to the map being checked.

  CheckerSettings settings_;  ///< Settings for the checker.
};

/**
 * @brief Generates a point with a unit Jacobian.
 * @param map Map used for the generation.
 * @param point Initial point.
 * @return Optional point with a unit Jacobian.
 */
inline std::optional<Point> GeneratePointWithSpecificJacobian(
    const Map& map, const Point& point, const Complex& value)
{
  VLOG(4) << "Generating point with specific Jacobian: " << value;
  const auto dimensions = map.GetDimensions();

  auto jacobian = map.GetJacobian();

  std::vector<Polynomial> equations;

  std::ranges::transform(
      Symbols::GetSymbolsList(dimensions), point, std::back_inserter(equations),
      [](const auto& symbol, const auto& coord) { return symbol - coord; });

  for (size_t i = 0; i < dimensions; ++i)
  {
    Equation equation;
    for (size_t j = 0; j < dimensions; ++j)
    {
      if (j == i)
      {
        equation.AppendEquation(jacobian - value);
      }
      else
      {
        equation.AppendEquation(equations[j]);
      }
    }
    if (!equation.IsSquare())
    {
      continue;
    }

    auto solutions = equation.Solve();
    auto solution = *solutions.begin();
    DLOG_IF(FATAL, solution.point.GetDimensions() != dimensions)
        << "The solution point does not have the expected dimensions.";
    VLOG(1) << "Using point with jacobian equal to "
            << ToStr(map.GetJacobian()
                         .subs(Symbols::GetSymbolsList(map.GetDimensions()),
                               solution.point.ToLst())
                         .evalf());
    VLOG(1) << "Point is " << solution.point.ToStr();

    return solution.point;
  }
  VLOG(1) << "Couldn't generate a point with jacobian equal to " << value;
  return {};
}
