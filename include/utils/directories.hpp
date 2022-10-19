#ifndef ASCPP_UTILS_DIRECTORIES_HPP_
#define ASCPP_UTILS_DIRECTORIES_HPP_

#include <cerrno>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

#include "utils/misc.hpp"
#include "utils/result.hpp"

namespace ascpp {

namespace fs = std::filesystem;  // NOLINT

inline auto GetConfigDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("APPDATA");
#elif defined(__unix__) || defined(__linux__)
  auto dir = GetEnv("XDG_CONFIG_HOME");
  if (dir) {
    return dir;
  }
  dir = GetEnv("HOME");
  if (dir) {
    return fs::path{dir.Unwrap()} / ".config";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = GetEnv("HOME");
  if (dir) {
    return fs::path{dir.Unwrap()} / "Library/Application Support";
  }
  return dir;
#endif
}

inline auto GetCacheDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("LOCALAPPDATA");
#elif defined(__unix__) || defined(__linux__)
  auto dir = GetEnv("XDG_CACHE_HOME");
  if (dir) {
    return dir;
  }
  dir = GetEnv("HOME");
  if (dir) {
    return fs::path{dir.Unwrap()} / ".cache";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  if (auto dir = GetEnv("HOME"); dir) {
    return fs::path{dir.Unwrap()} / "Library/Caches";
  }
  return dir;
#endif
}

inline auto GetHomeDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("USERPROFILE");
#else
  return GetEnv("HOME");
#endif
}

inline auto CreateFilePath(const std::filesystem::path& filepath) -> std::error_code {
  std::error_code err{};
  auto result = std::filesystem::exists(filepath, err);
  if (err) {
    return err;
  }
  if (result) {
    return err;
  }
  result = std::filesystem::exists(filepath.parent_path(), err);
  if (err) {
    return err;
  }
  if (!result) {
    std::filesystem::create_directories(filepath.parent_path(), err);
    if (err) {
      return err;
    }
  }
  const std::ofstream out{filepath};
  if (!out) {
    return std::make_error_code(static_cast<std::errc>(errno));
  }
  return err;
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_DIRECTORIES_HPP_
