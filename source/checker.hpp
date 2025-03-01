#pragma once

#include <ginac/ex.h>
#include <ginac/numeric.h>
#include <glog/logging.h>

#include <algorithm>
#include <iterator>
#include <optional>
#include <vector>

#include "equation.hpp"
#include "map.hpp"
#include "point.hpp"
#include "solver.hpp"
#include "symbol.hpp"

struct CheckerSettings
{
  size_t iterations = 10;
  double epsilon = 1e-4;
  bool check_contraction = true;
};

class CheckResult
{
 public:
  struct Error
  {
    std::string message;

    std::string ToStr() const { return "Error. " + message; }
  };

  struct Failed
  {
    Failed(Point p, Complex v) : point(std::move(p)), value(std::move(v)) {}

    Failed(Failed&&) noexcept = default;
    Failed& operator=(Failed&&) noexcept = default;

    std::string ToStr() const
    {
      return "Failed. Point: " + point.ToStr() + ", Value: " + ::ToStr(value);
    }

    Point point;
    Complex value;
  };

  struct Passed
  {
    std::string ToStr() const
    {
      return "Passed. Total checks: " + std::to_string(check_amount);
    }
    size_t check_amount;
  };

  struct Contraction
  {
    std::string ToStr() const
    {
      return "Contraction. At point: " + point.ToStr();
    }

    Point point;
  };
  using Result = std::variant<Error, Failed, Passed, Contraction>;

  CheckResult(Map m, Result res) : map(std::move(m)), result(std::move(res)) {}

  const Map& GetMap() const { return map; }
  const Result& GetResult() const { return result; }
  bool IsSuccessful() const { return std::holds_alternative<Passed>(result); }

  std::string ToStr() const
  {
    return "Map: " + map.ToStr() + ". Result: " +
           std::visit([](auto&& arg) { return arg.ToStr(); }, result);
  }

 private:
  Map map;
  Result result;
};
std::optional<Point> GeneratePointWithUnitJacobian(const Map& map,
                                                   const Point& point);

class Checker
{
 public:
  explicit Checker(const CheckerSettings& settings = CheckerSettings())
      : settings_(settings), map_(nullptr)
  {}

  void SetMap(const Map* new_map)
  {
    VLOG(0) << "Setting new map.";
    map_ = new_map;
  }

  CheckResult::Result PerformCheck()
  {
    LOG_IF(FATAL, !map_) << "Attempting to perform a check without a map set.";

    try
    {
      if (auto point = map_->HasContraction())
      {
        return CheckResult::Contraction{*point};
      }

      for (size_t iteration = 0; iteration < settings_.iterations; ++iteration)
      {
        Point point = Point::GenerateRandom(map_->GetDimensions());
        point = map_->Image(
            GeneratePointWithUnitJacobian(*map_, point).value_or(point));

        auto result = TestPoint(point);
        if (result)
        {
          return CheckResult::Failed{point, *result};
        }
      }
    }
    catch (const std::exception& e)
    {
      LOG(ERROR) << "Exception occurred: " << e.what();
      return CheckResult::Error{e.what()};
    }
    return CheckResult::Passed{settings_.iterations};
  }

 private:
  std::optional<Complex> TestPoint(const Point& point) const
  {
    VLOG(0) << "Testing point: " << point.ToStr();
    auto solutions = Solver::GetInstance().Solve(*map_, point);

    Complex sum_reciprocals = 0;

    for (const auto& [multiplicity, solution] : solutions)
    {
      auto jacobian = map_->GetJacobian();
      auto term =
          multiplicity / SubstituteJacobian(std::move(jacobian), solution);
      sum_reciprocals += term;
    }

    auto approximate_sum = GiNaC::abs(sum_reciprocals).to_double();
    VLOG(1) << "Sum of reciprocals of Jacobians: " << approximate_sum;

    bool result = approximate_sum < settings_.epsilon;
    VLOG(1) << "Test result: " << (result ? "passed" : "failed");
    return result ? std::nullopt : std::optional<Complex>{approximate_sum};
  }

  Complex SubstituteJacobian(Polynomial jacobian, const Point& point) const
  {
    return GiNaC::ex_to<Complex>(jacobian.subs(
        Symbols::GetSymbolsList(map_->GetDimensions()), point.ToLst()));
  }

  const Map* map_ = nullptr;

  CheckerSettings settings_;
};

inline std::optional<Point> GeneratePointWithUnitJacobian(const Map& map,
                                                          const Point& point)
{
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
        equation.AppendEquation(jacobian - 1);
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
  VLOG(1) << "Couldn't generate a point with jacobian equal to 1.";
  return {};
}
