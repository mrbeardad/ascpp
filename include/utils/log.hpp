#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include "spdlog/logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace ascpp {

class Logger {
 public:
 private:
  explicit Logger(std::shared_ptr<spdlog::logger> logger) : logger_{std::move(logger)} {}

  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  auto operator=(Logger&&) -> Logger& = default;
  auto operator=(const Logger&) -> Logger& = default;
  ~Logger() = default;

  auto LogInfo(std::string msg) -> void { logger_->info(msg); }

 private:
  std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace ascpp
