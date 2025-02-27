#pragma once

#include <cassert>
#include <iostream>
#include <istream>
#include <ostream>

#include "checker.hpp"
#include "map.hpp"
#include "map_factory.hpp"
#include "source/point_factory.hpp"

class UserInputMapGenerator
{
 public:
  UserInputMapGenerator(std::istream& input_stream, std::ostream& output_stream)
      : input(input_stream), out(output_stream)
  {}

  Map Generate() const
  {
    out << "Input your map:\n";
    return MapFactory::CreateMapFromInput(input, out);
  }

 private:
  std::istream& input;
  std::ostream& out;
};


class InvJacApp
{
 public:
  explicit InvJacApp(std::istream& input_stream = std::cin,
                     std::ostream& output_stream = std::cout)
      : input(input_stream), out(output_stream)
  {}

  explicit InvJacApp(std::ostream& output_stream) : out(output_stream) {}

  void Run()
  {
    WriteWelcomeMessage();

    MapGenerator map_generator = GetMapGenerator();
    PointGenerator point_generator = GetPointGenerator();

    Map map = map_generator();
    Point point = point_generator(map);
  }

 private:
  using MapGenerator = std::function<Map(void)>;
  using PointGenerator = std::function<Point(const Map&)>;

  void WriteWelcomeMessage() const
  {
    out << "Welcome to the Inverse Jacobian Application!\n";
  }

  bool AskForUserInput() const
  {
    out << "Would you like to input your own map? (Y/n): ";
    char user_input;
    input >> user_input;
    return (user_input == 'y' || user_input == 'Y' || user_input == '\n');
  }

  MapGenerator GetMapGenerator() const
  {
    if (AskForUserInput())
    {
      return [this]() -> Map
      {
        out << "Input your map:\n";
        return MapFactory::CreateMapFromInput(input, out);
      };
    }

    assert(false && "Default map generation not implemented.");
  }

  enum class PointType
  {
    UserInputPoint,
    RandomPoint,
    PointWithUnitJacobian
  };

  PointType AskTypeOfPoint() const
  {
    out << "Select the type of point (1: User Input, 2: Random point, 3: "
           "Random point with unit jacobian): ";
    int user_input;
    while (true)
    {
      input >> user_input;

      switch (user_input)
      {
        using enum PointType;
        case 1:
          return UserInputPoint;
        case 2:
          return RandomPoint;
        case 3:
          return PointWithUnitJacobian;
        default:
          out << "Invalid selection. Please try again.\n";
      }
    }
  }

  PointGenerator GetPointGenerator() const
  {
    switch (AskTypeOfPoint())
    {
      using enum PointType;
      case UserInputPoint:
        return [this](const Map& map) {
          return PointFactory::CreatePointFromInput(input, out,
                                                    map.GetDimensions());
        };
      case RandomPoint:
        out << "Using random point initialization.\n";
        return [](const Map& map)
        { return Point::GenerateRandom(map.GetDimensions()); };
      case PointWithUnitJacobian:
        out << "Using random point with unit Jacobian initialization.\n";
        return [](const Map& map)
        {
          auto random_point = Point::GenerateRandom(map.GetDimensions());
          return GeneratePointWithUnitJacobian(map, random_point)
              .value_or(random_point);
        };
      default:
        LOG(FATAL) << "Invalid point type selected";
    }
  }

  std::istream& input = std::cin;
  std::ostream& out = std::cout;
};