#include "map_factory.hpp"

#include <ginac/parse_context.h>

#include <ostream>

#include "symbol.hpp"

Map MapFactory::CreateMapFromInput(std::istream& input, std::ostream& out)
{
  out << "Enter amount of dimensions:\n";
  size_t dimensions;
  input >> dimensions;
  Symbols::FitInto(dimensions);

  std::vector<Polynomial> polynomials(dimensions);
  static GiNaC::parser parser(Symbols::GetSymtable(dimensions));

  out << "Please provide the polynomials, one per line:\n";
  for (auto& poly : polynomials)
  {
    poly = parser(input);
  }

  return Map(std::move(polynomials));
}
