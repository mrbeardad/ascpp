#pragma once

namespace ascpp {

class Logger {
 public:
  Logger() = default;
  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  auto operator=(Logger&&) -> Logger& = default;
  auto operator=(const Logger&) -> Logger& = default;
  ~Logger() = default;

 private:
};

}  // namespace ascpp
