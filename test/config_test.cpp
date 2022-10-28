#include "storage/config.hpp"

#include "gtest/gtest.h"

#include "app.hpp"

TEST(TestConfig, BasicUsage) {
  ascpp::Config cfg{};
  auto init_res = cfg.Init(app_info, ascpp::Config::kUserConfig);
  if (init_res.IsErr()) {
    std::cout << "fuck" << std::endl;
  }
  cfg["fuck"] = "you";
  cfg.WriteConfigToSystem();
}
