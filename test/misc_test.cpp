#include "utils/misc.hpp"

#include <iostream>
#include <limits>
#include <ranges>
#include <string>

#include "fmt/core.h"
#include "gtest/gtest.h"

TEST(TestMisc, ToInt) {
  EXPECT_EQ(ascpp::to_int("0b10"), 2);
  EXPECT_EQ(ascpp::to_int("0B10"), 2);
  EXPECT_EQ(ascpp::to_int("010"), 8);
  EXPECT_EQ(ascpp::to_int("0o10"), 8);
  EXPECT_EQ(ascpp::to_int("0O10"), 8);
  EXPECT_EQ(ascpp::to_int("0x10"), 16);
  EXPECT_EQ(ascpp::to_int("0X10"), 16);
  EXPECT_EQ(ascpp::to_int("10"), 10);
  EXPECT_EQ(ascpp::to_int("0"), 0);
  EXPECT_EQ(ascpp::to_int("+0"), 0);
  EXPECT_EQ(ascpp::to_int("-0"), 0);
  EXPECT_EQ(ascpp::to_int("+2147483647"), std::numeric_limits<int>::max());
  EXPECT_EQ(ascpp::to_int("-2147483648"), std::numeric_limits<int>::min());
  EXPECT_ANY_THROW(ascpp::to_int("+2147483648"));
  EXPECT_ANY_THROW(ascpp::to_int("-2147483649"));
  EXPECT_ANY_THROW(ascpp::to_int(""));
  EXPECT_ANY_THROW(ascpp::to_int("10x"));
  EXPECT_ANY_THROW(ascpp::to_int("x10"));
}

TEST(TestMisc, ToUll) {
  EXPECT_EQ(ascpp::to_ull("0b10"), 2);
  EXPECT_EQ(ascpp::to_ull("0B10"), 2);
  EXPECT_EQ(ascpp::to_ull("010"), 8);
  EXPECT_EQ(ascpp::to_ull("0o10"), 8);
  EXPECT_EQ(ascpp::to_ull("0O10"), 8);
  EXPECT_EQ(ascpp::to_ull("0x10"), 16);
  EXPECT_EQ(ascpp::to_ull("0X10"), 16);
  EXPECT_EQ(ascpp::to_ull("10"), 10);
  EXPECT_EQ(ascpp::to_ull("0"), 0);
  EXPECT_EQ(ascpp::to_ull("+0"), 0);
  EXPECT_EQ(ascpp::to_ull("-0"), 0);
  EXPECT_EQ(ascpp::to_ull("+18446744073709551615"), std::numeric_limits<unsigned long long>::max());
  EXPECT_EQ(ascpp::to_ull("-1"), std::numeric_limits<unsigned long long>::max());
  EXPECT_EQ(ascpp::to_ull("-18446744073709551615"), 1);
  EXPECT_ANY_THROW(ascpp::to_ull("+18446744073709551616"));
  EXPECT_ANY_THROW(ascpp::to_ull("-18446744073709551616"));
  EXPECT_ANY_THROW(ascpp::to_ull(""));
  EXPECT_ANY_THROW(ascpp::to_ull("10x"));
  EXPECT_ANY_THROW(ascpp::to_ull("x10"));
}
