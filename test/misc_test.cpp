#include "utils/misc.hpp"

#include <iostream>
#include <limits>
#include <ranges>
#include <string>

#include "fmt/core.h"
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
  EXPECT_EQ(ascpp::Stoi("0"), 0);
  EXPECT_EQ(ascpp::Stoi("+0"), 0);
  EXPECT_EQ(ascpp::Stoi("-0"), 0);
  EXPECT_EQ(ascpp::Stoi("+2147483647"), std::numeric_limits<int>::max());
  EXPECT_EQ(ascpp::Stoi("-2147483648"), std::numeric_limits<int>::min());
  EXPECT_ANY_THROW(ascpp::Stoi("+2147483648"));
  EXPECT_ANY_THROW(ascpp::Stoi("-2147483649"));
  EXPECT_ANY_THROW(ascpp::Stoi(""));
  EXPECT_ANY_THROW(ascpp::Stoi("10x"));
  EXPECT_ANY_THROW(ascpp::Stoi("x10"));
}

TEST(TestMisc, Stoull) {
  EXPECT_EQ(ascpp::Stoull("0b10"), 2);
  EXPECT_EQ(ascpp::Stoull("0B10"), 2);
  EXPECT_EQ(ascpp::Stoull("010"), 8);
  EXPECT_EQ(ascpp::Stoull("0o10"), 8);
  EXPECT_EQ(ascpp::Stoull("0O10"), 8);
  EXPECT_EQ(ascpp::Stoull("0x10"), 16);
  EXPECT_EQ(ascpp::Stoull("0X10"), 16);
  EXPECT_EQ(ascpp::Stoull("10"), 10);
  EXPECT_EQ(ascpp::Stoull("0"), 0);
  EXPECT_EQ(ascpp::Stoull("+0"), 0);
  EXPECT_EQ(ascpp::Stoull("-0"), 0);
  EXPECT_EQ(ascpp::Stoull("+18446744073709551615"), std::numeric_limits<unsigned long long>::max());
  EXPECT_EQ(ascpp::Stoull("-1"), std::numeric_limits<unsigned long long>::max());
  EXPECT_EQ(ascpp::Stoull("-18446744073709551615"), 1);
  EXPECT_ANY_THROW(ascpp::Stoull("+18446744073709551616"));
  EXPECT_ANY_THROW(ascpp::Stoull("-18446744073709551616"));
  EXPECT_ANY_THROW(ascpp::Stoull(""));
  EXPECT_ANY_THROW(ascpp::Stoull("10x"));
  EXPECT_ANY_THROW(ascpp::Stoull("x10"));
}
