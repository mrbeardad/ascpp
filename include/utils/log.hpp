#pragma once

#include "app/info.hpp"

class logger {
 public:
  enum level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

  explicit logger(const ascpp::app_info* info) : _app_info(info) {}

  logger(logger&&) = default;
  logger(const logger&) = default;
  auto operator=(logger&&) -> logger& = default;
  auto operator=(const logger&) -> logger& = default;
  ~logger() = default;

 private:
  const ascpp::app_info* _app_info;
};
