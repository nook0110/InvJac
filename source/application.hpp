#pragma once

#include <iostream>
#include <ostream>

#include "map.hpp"
#include "map_factory.hpp"

class InvJacApp
{
 public:
  InvJacApp();

  void Run()
  {
    WriteWelcomeMessage();
    if (AskForUserInput())
    {
      RunUserInput();
    }
    else
    {
    }
  }

 private:
  std::istream& input = std::cin;
  std::ostream& out = std::cout;

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

  bool AskForPoint() const
  {
    out << "Would you like to ask for a point? (Y/n): ";
    char user_input;
    input >> user_input;
    return (user_input == 'y' || user_input == 'Y' || user_input == '\n');
  }

  Point GetPointInput() const
  {
    out << "Please enter the coordinates of the point: ";
    Point point;

    input >> point;
    return point;
  }

  void RunUserInput() const
  {
    Map userMap = GetUserInputMap();

    if (AskForPoint())
    {
    }
  }
};