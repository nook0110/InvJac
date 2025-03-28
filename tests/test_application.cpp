#include <gtest/gtest.h>

#include <sstream>

#include "source/application.hpp"

class InvJacAppTest : public ::testing::Test
{
 protected:
  std::stringstream input_stream;
  std::stringstream output_stream;
  InvJacApp app{input_stream, output_stream};
};

TEST_F(InvJacAppTest, HelpMessageDisplayed)
{
  const char* argv[] = {"program", "--help"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  bool help_displayed = app.ParseCommandLine(argc, const_cast<char**>(argv));
  EXPECT_TRUE(help_displayed);
  EXPECT_NE(output_stream.str().find("Allowed options"), std::string::npos);
}

TEST_F(InvJacAppTest, InputFileError)
{
  const char* argv[] = {"program", "--input-file", "nonexistent.txt"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  bool result = app.ParseCommandLine(argc, const_cast<char**>(argv));
  EXPECT_FALSE(result);
  EXPECT_NE(output_stream.str().find("Error opening input file"),
            std::string::npos);
}

TEST_F(InvJacAppTest, RunApplicationWithoutDatabase)
{
  input_stream << "2\n";
  input_stream << "x^2+1\n";
  input_stream << "y^2+1";
  app.Run();

  EXPECT_NE(output_stream.str().find("Running the InvJac application"),
            std::string::npos);
  EXPECT_NE(output_stream.str().find("Application finished"),
            std::string::npos);
}
