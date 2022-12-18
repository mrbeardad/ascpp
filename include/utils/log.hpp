#pragma once

#include "app/info.hpp"

class Logger {
 public:
  explicit Logger(const ascpp::app_info* info) : app_info_(info) {}

  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  auto operator=(Logger&&) -> Logger& = default;
  auto operator=(const Logger&) -> Logger& = default;
  ~Logger() = default;

 private:
  const ascpp::app_info* app_info_;
};
