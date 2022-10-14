#ifndef ASCPP_UTILS_DIRECTORIES_HPP_
#define ASCPP_UTILS_DIRECTORIES_HPP_

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>

#include "utils/misc.hpp"

namespace ascpp {

namespace fs = std::filesystem;

inline std::optional<std::string> GetConfigDir() {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("APPDATA");
#elif defined(__unix__) || defined(__linux__)
  if (auto dir = GetEnv("XDG_CONFIG_HOME"); dir) {
    return dir;
  }
  if (auto dir = GetEnv("HOME"); dir) {
    fs::path dirpath{dir.value()};
    dirpath /= ".config";
    return dirpath.string();
  }
  return {};
#elif defined(TARGET_OS_MAC)
  if (auto dir = GetEnv("HOME"); dir) {
    fs::path dirpath{dir.value()};
    dirpath /= "Library/Application Support";
    return dirpath.string();
  }
  return {};
#endif
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_DIRECTORIES_HPP_
