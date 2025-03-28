#include <gtest/gtest.h>

#include "source/phc_wrapper.hpp"

TEST(PHCWrapperTest, InitializeNumberOfPolynomials) {}

TEST(PHCWrapperTest, InsertPolynomial)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  wrapper.InitializeNumberOfPolynomials(2);
  ASSERT_NO_THROW(wrapper.InsertPolynomial("x*y - 2;", 0););
  ASSERT_NO_THROW(wrapper.InsertPolynomial("x^2 - 1;", 1););
}

TEST(PHCWrapperTest, GetAmountOfSymbols)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  wrapper.InitializeNumberOfPolynomials(2);
  wrapper.InsertPolynomial("x*y - 2;", 0);
  wrapper.InsertPolynomial("x^2 - 1;", 1);
  ASSERT_EQ(wrapper.GetAmountOfSymbols(), 2);
}

TEST(PHCWrapperTest, IsSquare)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  wrapper.InitializeNumberOfPolynomials(2);
  wrapper.InsertPolynomial("x*y - 2;", 0);
  wrapper.InsertPolynomial("x^2 - 1;", 1);
  ASSERT_TRUE(wrapper.IsSquare());
}

TEST(PHCWrapperTest, ParseRoot)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  // clang-format off
  std::string solution_string =
    "t :  0.00000000000000E+00   0.00000000000000E+00\n"
    "m : 1\n"
    "the solution for t :\n"
    " x_0 : 9.96676921350590249034125378116507E-03      1.46150526361638135612257536395424E-01    \n"
    " x_1 : 1.15374259155635172650000000000000E+00      -1.07774252644852808200000000000001E+00   \n"
    "== err :  1.233E-32 = rco :  3.127E-02 = res :  5.065E-32 =";
  // clang-format on

  const phc::PHCWrapper::Root answer = {
      1,
      {{"9.96676921350590249034125378116507E-03",
        "1.46150526361638135612257536395424E-01"},
       {"1.15374259155635172650000000000000E+00",
        "-1.07774252644852808200000000000001E+00"}}};

  auto parsed_root = wrapper.ParseRoot(solution_string, 100);
  ASSERT_EQ(answer, parsed_root);
}

TEST(PHCWrapperTest, Solve)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  wrapper.InitializeNumberOfPolynomials(2);
  wrapper.InsertPolynomial("x*y - 2;", 0);
  wrapper.InsertPolynomial("x^2 - 1;", 1);
  auto solutions = wrapper.Solve();

  ASSERT_EQ(solutions.size(), 2);
}
