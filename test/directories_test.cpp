#include "utils/directories.hpp"

#include <stdlib.h>
#include <iostream>
#include <ostream>

#include "gtest/gtest.h"

TEST(TestDirectories, GetConfigDir) {
  std::cout << "GetHomeDir()  : " << ascpp::GetHomeDir().Unwrap() << std::endl;
  std::cout << "GetConfigDir(): " << ascpp::GetConfigDir().Unwrap() << std::endl;
  std::cout << "GetCacheDir() : " << ascpp::GetCacheDir().Unwrap() << std::endl;
}
