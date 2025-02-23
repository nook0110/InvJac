#include "point.hpp"

GiNaC::lst Point::ToLst() const
{
  GiNaC::lst result;
  for (const auto& coord : coords_)
  {
    result.append(coord);
  }
  return result;
}