#pragma once

#include <iostream>
#include <istream>
#include <ostream>

#include "checker.hpp"
#include "map.hpp"
#include "map_factory.hpp"
#include "source/point_factory.hpp"

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
    if (AskForUserInput())
    {
      RunUserInput();
    }
    else
    {
      return;
    }
  }

 private:
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

  Map GetUserInputMap() const
  {
    out << "Input your map:\n";
    return MapFactory::CreateMapFromInput(input, out);
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

  Point GetPointInput(size_t dimensions) const
  {
    out << "Please enter the coordinates of the point:\n";
    Point point = PointFactory::CreateMapFromInput(input, dimensions);

    return point;
  }

  void RunUserInput() const
  {
    Map user_map = GetUserInputMap();
    Point point;
    switch (AskTypeOfPoint())
    {
      using enum PointType;
      case UserInputPoint:
        point = GetPointInput(user_map.GetDimensions());
        break;
      case RandomPoint:
        out << "Using random point initialization.\n";
        point = Point::GenerateRandom(user_map.GetDimensions());
        break;
      case PointWithUnitJacobian:
        out << "Using random point with unit Jacobian initialization.\n";
        auto random_point = Point::GenerateRandom(user_map.GetDimensions());
        point = GeneratePointWithUnitJacobian(user_map, random_point)
                    .value_or(random_point);
        break;
    }

    out << "Using point: " << point.ToStr() << "\n";
  }

  std::istream& input = std::cin;
  std::ostream& out = std::cout;
};