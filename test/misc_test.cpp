#include "utils/misc.hpp"

#include "gtest/gtest.h"

TEST(TestMisc, GetEnv) {
  EXPECT_FALSE(ascpp::GetEnv("null"));

  ascpp::SetEnv("empty", "");
  EXPECT_FALSE(ascpp::GetEnv("empty"));

  ascpp::SetEnv("hello", "world");
  EXPECT_EQ(ascpp::GetEnv("hello").Unwrap(), "world");

  ascpp::SetEnv("name with space", "value with space");
  EXPECT_EQ(ascpp::GetEnv("name with space").Unwrap(), "value with space");
}
