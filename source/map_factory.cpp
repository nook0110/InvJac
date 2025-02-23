#include "map_factory.hpp"

#include "symbol.hpp"

Map MapFactory::CreateMapFromInput(std::istream& input)
{
  size_t dimensions;
  input >> dimensions;
  Symbols::FitInto(dimensions);

  std::vector<Polynomial> polynomials(dimensions);
  std::string line;
  std::getline(input, line);  // consume the remaining newline

  for (auto& poly : polynomials)
  {
    std::getline(input, line);
    poly = GiNaC::ex(line, Symbols::GetSymbolsList());
  }

  return Map(std::move(polynomials));
}
