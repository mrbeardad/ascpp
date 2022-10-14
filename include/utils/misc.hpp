#ifndef ASCPP_UTILS_MISC_HPP_
#define ASCPP_UTILS_MISC_HPP_

#include <cstdlib>
#include <optional>
#include <string>

namespace ascpp {

inline std::optional<std::string> GetEnv(const std::string& name) {
  size_t size{};
  auto err = ::getenv_s(&size, nullptr, 0, name.c_str());
  if (err || size == 0) {
    return {};
  }

  std::string result(size, '\0');
  err = ::getenv_s(&size, result.data(), size, name.c_str());
  if (err) {
    return {};
  }
  result.resize(size - 1);
  return result;
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_MISC_HPP_
