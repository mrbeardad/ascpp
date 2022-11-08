#include "utils/system.hpp"

#include "gtest/gtest.h"

TEST(TestMisc, GetAndSetEnv) {
  EXPECT_FALSE(ascpp::GetEnv("null").IsOk());

  ascpp::SetEnv("empty", "");
  EXPECT_FALSE(ascpp::GetEnv("empty").IsOk());

  ascpp::SetEnv("hello", "world");
  EXPECT_EQ(ascpp::GetEnv("hello").Unwrap(), "world");

  ascpp::SetEnv("name with space", "value with space");
  EXPECT_EQ(ascpp::GetEnv("name with space").Unwrap(), "value with space");
}

TEST(TestDirectories, Directories) {
  // std::cout << "GetHomeDir()  : " << ascpp::GetHomeDir().Unwrap() << std::endl;
  // std::cout << "GetConfigDir(): " << ascpp::GetConfigDir().Unwrap() << std::endl;
  // std::cout << "GetCacheDir() : " << ascpp::GetCacheDir().Unwrap() << std::endl;
  // std::cout << "CreateFilePath(): " << ascpp::GetCacheDir().Unwrap() / "ascpp/" << std::endl;
  // ascpp::CreateFilePath(ascpp::GetCacheDir().Unwrap() / "ascpp");
  // std::cout << "CreateFilePath(): " << ascpp::GetCacheDir().Unwrap() / "ascpp/file.txt"
  //           << std::endl;
  // ascpp::CreateFilePath(ascpp::GetCacheDir().Unwrap() / "ascpp/file.txt");
}
