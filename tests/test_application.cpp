#include <gtest/gtest.h>

#include <sstream>

#include "source/application.hpp"

TEST(InvJacAppTest, RunUserInput)
{
  std::stringstream input(
      "Y\n"
      "2\n"
      "x^2\n"
      "y^2\n"
      "1\n"
      "1.0\n"
      "2.0\n");
  std::stringstream output;
  InvJacApp app(input, output);

  app.Run();
  EXPECT_NE(output.str().find("Using point: (1.0, 2.0)"), std::string::npos);
}
