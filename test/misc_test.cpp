#include "utils/misc.hpp"
#include <stdlib.h>

#include "gtest/gtest.h"

TEST(TestMisc, GetEnv) {
  EXPECT_FALSE(ascpp::GetEnv("null"));

  ascpp::SetEnv("empty", "");
  EXPECT_FALSE(ascpp::GetEnv("empty"));

  ascpp::SetEnv("hello", "world");
  EXPECT_EQ(ascpp::GetEnv("hello").value(), "world");

  ascpp::SetEnv("name with space", "value with space");
  EXPECT_EQ(ascpp::GetEnv("name with space").value(), "value with space");
}
