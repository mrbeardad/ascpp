#include "utils/misc.hpp"

#include "gtest/gtest.h"

TEST(TestMisc, Stoi) {
  EXPECT_EQ(ascpp::Stoi("0b10"), 2);
  EXPECT_EQ(ascpp::Stoi("0B10"), 2);
  EXPECT_EQ(ascpp::Stoi("010"), 8);
  EXPECT_EQ(ascpp::Stoi("0o10"), 8);
  EXPECT_EQ(ascpp::Stoi("0O10"), 8);
  EXPECT_EQ(ascpp::Stoi("0x10"), 16);
  EXPECT_EQ(ascpp::Stoi("0X10"), 16);
  EXPECT_EQ(ascpp::Stoi("10"), 10);
  EXPECT_ANY_THROW(ascpp::Stoi("10x"));
  EXPECT_ANY_THROW(ascpp::Stoi("x10"));
}
