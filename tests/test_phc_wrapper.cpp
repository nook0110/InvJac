#include <gtest/gtest.h>

#include "source/phc_wrapper.hpp"

TEST(PHCWrapperTest, InitializeNumberOfPolynomials)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  ASSERT_NO_THROW(wrapper.InitializeNumberOfPolynomials(3));
}

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

TEST(PHCWrapperTest, Solve)
{
  const auto& wrapper = phc::PHCWrapper::GetInstance();
  wrapper.InitializeNumberOfPolynomials(2);
  wrapper.InsertPolynomial("x*y - 2;", 0);
  wrapper.InsertPolynomial("x^2 - 1;", 1);
  auto solutions = wrapper.Solve();

  ASSERT_EQ(solutions.size(), 2);
}
