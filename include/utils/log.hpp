#ifndef ASCPP_UTILS_LOG_HPP_
#define ASCPP_UTILS_LOG_HPP_

#include "async/app.hpp"

namespace ascpp {

class Logger {
 public:
  Logger();
  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  auto operator=(Logger&&) -> Logger& = default;
  auto operator=(const Logger&) -> Logger& = default;
  ~Logger();

 private:
};

}  // namespace ascpp

#endif  // !ASCPP_UTILS_LOG_HPP_
