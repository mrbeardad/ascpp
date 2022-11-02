#pragma once

#include <algorithm>
#include "spdlog/logger.h"
#include "spdlog/spdlog.h"

namespace ascpp {

class Logger {
 public:
  explicit Logger(spdlog::logger logger) : logger_{std::move(logger)} {}

  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  auto operator=(Logger&&) -> Logger& = default;
  auto operator=(const Logger&) -> Logger& = default;
  ~Logger() = default;

 private:
  spdlog::logger logger_;
};

}  // namespace ascpp
