#include <ginac/ex.h>
#include <ginac/numeric.h>
#include <glog/logging.h>

#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include "map.hpp"
#include "point.hpp"
#include "solver.hpp"
#include "source/phc_wrapper.hpp"
#include "symbol.hpp"
class Checker
{
 public:
  struct Settings
  {
    size_t iterations = 10;
    double epsilon = 1e-4;
  };

  void SetMap(const Map* new_map)
  {
    VLOG(0) << "Setting new map.";
    map_ = new_map;
  }

  std::optional<Complex> TestPoint(const Point& point) const
  {
    VLOG(0) << "Testing point: " << point.ToStr();
    auto solutions = Solver::GetInstance().Solve(*map_, point);

    GiNaC::numeric sum_reciprocals = 0;

    for (const auto& [multiplicity, solution] : solutions)
    {
      auto jacobian = map_->GetJacobian();
      auto term =
          multiplicity / SubstituteJacobian(std::move(jacobian), solution);
      sum_reciprocals += term;
    }

    GiNaC::numeric approximate_sum = GiNaC::abs(sum_reciprocals).to_double();

    VLOG(1) << "Sum of reciprocals of Jacobians: " << approximate_sum;

    bool result = approximate_sum < settings_.epsilon;
    VLOG(1) << "Test result: " << (result ? "passed" : "failed");
    return result ? std::nullopt : std::optional<Complex>{approximate_sum};
  }

 private:
  Complex SubstituteJacobian(Polynomial jacobian, const Point& point) const
  {
    return GiNaC::ex_to<Complex>(jacobian.subs(
        Symbols::GetSymbolsList(map_->GetDimensions()), point.ToLst()));
  }

  const Map* map_;
  Settings settings_;
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
    phc::Equation equation;
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
