#include "point_factory.hpp"

#include <ginac/ex.h>
#include <ginac/parser.h>

#include "source/point.hpp"

Point PointFactory::CreatePointFromInput(std::istream& input,
                                         [[maybe_unused]] std::ostream& output,
                                         size_t dimensions)
{
  std::vector<Complex> coords;
  coords.reserve(dimensions);

  GiNaC::parser parser;
  for (size_t i = 0; i < dimensions; ++i)
  {
    std::string input_string;
    input >> input_string;
    GiNaC::ex e = parser(input_string);
    coords.emplace_back(GiNaC::ex_to<Complex>(e));
  }

  return Point(std::move(coords));
}
