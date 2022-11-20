#include "utils/system.hpp"
#include <filesystem>

#include "gtest/gtest.h"

TEST(TestMisc, GetAndSetEnv) {
  EXPECT_EQ(ascpp::GetEnv("null").UnwrapErr(), ascpp::SysErr::GET_EMPTY_ENV);

  ascpp::SetEnv("empty", "");
  EXPECT_EQ(ascpp::GetEnv("empty").UnwrapErr(), ascpp::SysErr::GET_EMPTY_ENV);

  ascpp::SetEnv("hello", "world");
  EXPECT_EQ(ascpp::GetEnv("hello").Unwrap(), "world");

  ascpp::SetEnv("name with space", "value with space");
  EXPECT_EQ(ascpp::GetEnv("name with space").Unwrap(), "value with space");
}

TEST(TestDirectories, Directories) {
  std::clog << "GetHomeDir()  : " << ascpp::GetHomeDir().Unwrap() << std::endl;
  std::clog << "GetConfigDir(): " << ascpp::GetConfigDir().Unwrap() << std::endl;
  std::clog << "GetCacheDir() : " << ascpp::GetCacheDir().Unwrap() << std::endl;

  auto dirpath = ascpp::GetCacheDir().Unwrap() / "ascpp" / "";
  std::filesystem::remove_all(dirpath);
  std::clog << "CreateFilePath(): " << dirpath << std::endl;
  ascpp::CreateFilePath(dirpath);

  auto filepath = ascpp::GetCacheDir().Unwrap() / "ascpp" / "file.txt";
  std::filesystem::remove(filepath);
  std::clog << "CreateFilePath(): " << filepath << std::endl;
  ascpp::CreateFilePath(filepath);
}
