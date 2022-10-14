#ifndef ASCPP_UTILS_MISC_HPP_
#define ASCPP_UTILS_MISC_HPP_

#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>

namespace ascpp {

/**
 * @brief Get environment variable
 *
 * @param name[in] Variable name
 * @return std::optional<std::string> Variable value. Return nullopt if variable does not
 * exist or it is empty.
 */
inline std::optional<std::string> GetEnv(const std::string& name) {
#if defined(_WIN32) || defined(_WIN64)
  size_t size{};
  auto err = ::getenv_s(&size, nullptr, 0, name.c_str());
  if (err || size == 0) {
    return {};
  }

  std::string value(size, '\0');
  err = ::getenv_s(&size, value.data(), size, name.c_str());
  if (err) {
    return {};
  }
  value.resize(size - 1);
  return value;
#else
  auto* value = std::getenv(name.c_str());
  if (value == nullptr || std::strlen(value) == 0) {
    return {};
  }
  return value;
#endif
}

/**
 * @brief Set environment variable
 *
 * @param name[in] Variable name
 * @param value[in] Variable value
 * @return int Zero if it succeeds and nonzero otherwise
 */
inline int SetEnv(const std::string& name, const std::string& value) {
#if defined(_WIN32) || defined(_WIN64)
  return ::_putenv_s(name.c_str(), value.c_str());
#else
  return ::setenv(name.c_str(), value.c_str(), 1);
#endif
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_MISC_HPP_
