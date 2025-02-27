#include "point_factory.hpp"

#include <ginac/ex.h>
#include <ginac/parser.h>

#include "source/point.hpp"

Point PointFactory::CreateMapFromInput(std::istream& stream, size_t dimensions)
{
  std::vector<Complex> coords;
  coords.reserve(dimensions);

  GiNaC::parser parser;
  for (size_t i = 0; i < dimensions; ++i)
  {
    GiNaC::ex e = parser(stream);
    coords.emplace_back(GiNaC::ex_to<Complex>(e));
  }

  return Point(std::move(coords));
}
