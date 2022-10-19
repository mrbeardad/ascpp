#ifndef ASCPP_UTILS_MISC_HPP_
#define ASCPP_UTILS_MISC_HPP_

#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <system_error>
#include "utils/error.hpp"
#include "utils/result.hpp"

namespace ascpp {

inline auto GetEnv(const std::string& name) -> Result<std::string> {
#if defined(_WIN32) || defined(_WIN64)
  size_t size{};
  auto err = ::getenv_s(&size, nullptr, 0, name.c_str());
  if (err) {
    return make_error_code(SystemError::kGetEnvError);
  }
  if (size == 0) {
    return make_error_code(SystemError::kGetEnvWithEmptyVale);
  }

  std::string value(size, '\0');
  err = ::getenv_s(&size, value.data(), size, name.c_str());
  if (err) {
    return make_error_code(SystemError::kGetEnvError);
  }
  value.resize(size - 1);
  return value;
#else
  auto* value = std::getenv(name.c_str());
  if (value == nullptr) {
    return make_error_code(SystemError::kGetEnvError);
  }
  if (std::strlen(value) == 0) {
    return make_error_code(SystemError::kGetEnvWithEmptyVale);
  }
  return value;
#endif
}

inline auto SetEnv(const std::string& name, const std::string& value) -> std::error_code {
#if defined(_WIN32) || defined(_WIN64)
  if (::_putenv_s(name.c_str(), value.c_str())) {
    return make_error_code(SystemError::kSetEnvError);
  }
  return {};
#else
  if (::setenv(name.c_str(), value.c_str(), 1)) {
    return make_error_code(SystemError::kSetEnvError);
  }
  return {};
#endif
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_MISC_HPP_
