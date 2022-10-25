#ifndef ASCPP_UTILS_LOG_HPP_
#define ASCPP_UTILS_LOG_HPP_

#include "spdlog/sinks/basic_file_sink.h"

#include "async/app.hpp"

namespace ascpp {

class Logger {
 public:
  Logger();
  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  Logger& operator=(Logger&&) = default;
  Logger& operator=(const Logger&) = default;
  ~Logger();

 private:
  spdlog::logger logger_;
};

}  // namespace ascpp

#endif  // !ASCPP_UTILS_LOG_HPP_
