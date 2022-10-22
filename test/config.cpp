#include "storage/config.hpp"

#include "gtest/gtest.h"

#include "app.hpp"

TEST(TestConfig, BasicUsage) {
  ascpp::Config cfg{&app};
  auto init_res = cfg.Init(ascpp::Config::kUserConfig);
  if (init_res.IsErr()) {
    std::cout << "fuck" << std::endl;
  }
  cfg["fuck"] = "you";
  cfg.WriteConfigToSystem();
}
