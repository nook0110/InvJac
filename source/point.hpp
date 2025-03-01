#pragma once
#include <ginac/ginac.h>

#include <random>
#include <ranges>
#include <vector>

using Complex = GiNaC::numeric;

class Point
{
 public:
  explicit Point(std::vector<Complex> coords = {}) : coords_(std::move(coords))
  {}

  size_t GetDimensions() const { return coords_.size(); }

  const Complex& GetCoord(size_t i) const { return coords_[i]; }
  const Complex& operator[](size_t i) const { return GetCoord(i); }

  GiNaC::lst ToLst() const;

  static Point GenerateRandom(size_t dimensions)
  {
    std::vector<Complex> coords(dimensions);
    static std::mt19937 gen(80085);
    static std::uniform_real_distribution<double> dis(-10.0, 10.0);

    for (auto& coord : coords)
    {
      coord = dis(gen) + GiNaC::I * dis(gen);
    }
    return Point(std::move(coords));
  }

  std::string ToStr() const
  {
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < coords_.size(); ++i)
    {
      oss << coords_[i].evalf();
      if (i < coords_.size() - 1)
      {
        oss << ", ";
      }
    }
    oss << ")";
    return oss.str();
  }

  std::vector<Complex>::iterator begin() { return coords_.begin(); }
  std::vector<Complex>::const_iterator begin() const { return coords_.begin(); }
  std::vector<Complex>::iterator end() { return coords_.end(); }
  std::vector<Complex>::const_iterator end() const { return coords_.end(); }

 private:
  std::vector<Complex> coords_;
};

static_assert(std::ranges::range<Point>);
static_assert(std::ranges::constant_range<const Point>);
