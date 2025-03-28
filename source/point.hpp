#pragma once
#include <ginac/ginac.h>

#include <random>
#include <ranges>
#include <vector>

using Complex = GiNaC::numeric;

/**
 * @class Point
 * @brief Represents a point in a multidimensional complex space.
 */
class Point
{
 public:
  /**
   * @brief Constructs a Point with the given coordinates.
   * @param coords A vector of complex coordinates. Defaults to an empty vector.
   */
  explicit Point(std::vector<Complex> coords = {}) : coords_(std::move(coords))
  {}

  /**
   * @brief Gets the number of dimensions of the point.
   * @return The number of dimensions.
   */
  size_t GetDimensions() const { return coords_.size(); }

  /**
   * @brief Gets the coordinate at the specified index.
   * @param i The index of the coordinate.
   * @return A reference to the complex coordinate.
   */
  const Complex& GetCoord(size_t i) const { return coords_[i]; }

  /**
   * @brief Accesses the coordinate at the specified index.
   * @param i The index of the coordinate.
   * @return A reference to the complex coordinate.
   */
  const Complex& operator[](size_t i) const { return GetCoord(i); }

  /**
   * @brief Converts the point to a GiNaC::lst object.
   * @return A GiNaC::lst representation of the point.
   */
  GiNaC::lst ToLst() const;

  /**
   * @brief Generates a random point with the specified number of dimensions.
   * @param dimensions The number of dimensions for the point.
   * @return A randomly generated Point.
   */
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

  /**
   * @brief Converts the point to a string representation.
   * @return A string representation of the point.
   */
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

  /**
   * @brief Gets an iterator to the beginning of the coordinates.
   * @return An iterator to the beginning of the coordinates.
   */
  std::vector<Complex>::iterator begin() { return coords_.begin(); }

  /**
   * @brief Gets a constant iterator to the beginning of the coordinates.
   * @return A constant iterator to the beginning of the coordinates.
   */
  std::vector<Complex>::const_iterator begin() const { return coords_.begin(); }

  /**
   * @brief Gets an iterator to the end of the coordinates.
   * @return An iterator to the end of the coordinates.
   */
  std::vector<Complex>::iterator end() { return coords_.end(); }

  /**
   * @brief Gets a constant iterator to the end of the coordinates.
   * @return A constant iterator to the end of the coordinates.
   */
  std::vector<Complex>::const_iterator end() const { return coords_.end(); }

 private:
  std::vector<Complex> coords_;  ///< The coordinates of the point.
};

static_assert(std::ranges::range<Point>);
static_assert(std::ranges::constant_range<const Point>);
