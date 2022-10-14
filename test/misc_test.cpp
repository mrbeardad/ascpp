#include "utils/misc.hpp"
#include <stdlib.h>

#include "gtest/gtest.h"

TEST(Misc, GetEnv) {
  EXPECT_FALSE(ascpp::GetEnv("null"));

  ::_putenv_s("hello", "world");
  EXPECT_EQ(ascpp::GetEnv("hello").value(), "world");

  ::_putenv_s("empty", "");
  EXPECT_ANY_THROW(ascpp::GetEnv("empty").value());
}
