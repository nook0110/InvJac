#pragma once

#include <iostream>

#include "map.hpp"
#include "map_factory.hpp"

class InvJacApp
{
 public:
  InvJacApp();

  void Run();

 private:
  void WriteWelcomeMessage() const
  {
    std::cout << "Welcome to the Inverse Jacobian Application!" << std::endl;
  }

  bool AskForUserInput() const
  {
    std::cout << "Would you like to input your own map? (Y/n): ";
    char user_input;
    std::cin >> user_input;
    return (user_input == 'y' || user_input == 'Y' || user_input == '\n');
  }

  void RunUserInput() const
  {
    Map map = MapFactory::CreateMapFromInput(std::cin);
  }
};