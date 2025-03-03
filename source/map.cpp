#include "map.hpp"

#include <ginac/ex.h>
#include <ginac/matrix.h>

#include <algorithm>
#include <cstddef>

#include "point.hpp"
#include "solver.hpp"
#include "source/symbol.hpp"

Point Map::Image(const Point& point) const
{
  LOG_IF(FATAL, point.GetDimensions() != GetDimensions())
      << "Point and map must have same dimension!";

  std::vector<Complex> coords(GetDimensions());

  for (size_t i = 0; i < GetDimensions(); ++i)
  {
    coords[i] = GiNaC::ex_to<Complex>(GetPoly(i).subs(
        Symbols::GetSymbolsList(GetDimensions()), point.ToLst()));
  }

  return Point(std::move(coords));
}

void Map::EvaluateExtensionDegree() const
{
  size_t last_extension;
  do
  {
    last_extension = extension_degree_.value_or(0);
    auto random_point = Point::GenerateRandom(GetDimensions());
    extension_degree_ = std::max(
        last_extension,
        Solver::GetInstance().Solve(*this, random_point).GetRootCount<true>());
  }
  while (last_extension < extension_degree_);
}

std::optional<Point> Map::HasContraction() const
{
  DLOG_IF(FATAL, GetDimensions() != 2) << "Map must be 2-dimensional!";
  const auto jacobian_matrix = GetJacobianMatrix();
  const auto jacobian = jacobian_matrix.determinant();

  static std::random_device rd;
  static std::mt19937 gen(80085);
  static std::uniform_int_distribution dis(-10, 10);

  Equation equation;
  equation.AppendEquation(jacobian);
  if (jacobian.has(Symbols::GetSymbol(0)))
  {
    equation.AppendEquation(Symbols::GetSymbol(1) - dis(gen));
  }
  else if (jacobian.has(Symbols::GetSymbol(1)))
  {
    equation.AppendEquation(Symbols::GetSymbol(0) - dis(gen));
  }
  else
  {
    return {};
  }

  GiNaC::matrix gradient(GetDimensions(), 1);
  for (size_t row = 0; row < GetDimensions(); ++row)
  {
    gradient(row, 0) = jacobian.diff(Symbols::GetSymbol(row));
  }

  const auto normal = GiNaC::matrix({{gradient(1, 0)}, {-gradient(0, 0)}});

  const auto result = jacobian_matrix.mul(normal);

  for (const auto& [multiplicity, root] : equation.Solve())
  {
    auto result_at_point = result;
    for (size_t row = 0; row < GetDimensions(); ++row)
    {
      result_at_point(row, 0) = result_at_point(row, 0).subs(
          Symbols::GetSymbolsList(GetDimensions()), root.ToLst());
    }

    const auto epsilon = 1e-4;

    if (GiNaC::abs(result_at_point(0, 0)) < epsilon &&
        GiNaC::abs(result_at_point(1, 0)) < epsilon)
    {
      return root;
    }
  }

  return {};
}