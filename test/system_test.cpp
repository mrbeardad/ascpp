#include "utils/system.hpp"

#include <filesystem>
#include <iostream>
#include <ostream>

#include "gtest/gtest.h"

// NOLINTBEGIN(modernize-use-trailing-return-type)

TEST(TestMisc, GetAndSetEnv) {
  EXPECT_EQ(ascpp::get_env("null").error(), ascpp::error::get_empty_env);

  ascpp::set_env("empty", "").value();
  EXPECT_EQ(ascpp::get_env("empty").error(), ascpp::error::get_empty_env);

  ascpp::set_env("hello", "world").value();
  EXPECT_EQ(ascpp::get_env("hello").value(), "world");

  ascpp::set_env("name with space", "value with space").value();
  EXPECT_EQ(ascpp::get_env("name with space").value(), "value with space");
}

TEST(TestDirectories, Directories) {
  std::clog << "GetHomeDir()    : " << ascpp::get_home_dir().value() << std::endl;
  std::clog << "GetConfigDir()  : " << ascpp::get_config_dir().value() << std::endl;
  std::clog << "GetCacheDir()   : " << ascpp::get_cache_dir().value() << std::endl;

  auto dirpath = ascpp::get_cache_dir().value() / "ascpp" / "";
  std::filesystem::remove_all(dirpath);
  std::clog << "CreateFilePath(): " << dirpath << std::endl;
  ascpp::create_file_path(dirpath).value();

  auto filepath = ascpp::get_cache_dir().value() / "ascpp" / "file.txt";
  std::filesystem::remove(filepath);
  std::clog << "CreateFilePath(): " << filepath << std::endl;
  ascpp::create_file_path(filepath).value();
}

// NOLINTEND(modernize-use-trailing-return-type)
