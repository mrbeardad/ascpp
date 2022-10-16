#ifndef ASCPP_UTILS_DIRECTORIES_HPP_
#define ASCPP_UTILS_DIRECTORIES_HPP_

#include <cerrno>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>

#include "utils/misc.hpp"

namespace ascpp {

namespace fs = std::filesystem;

/**
 * @brief Get the configuration directory
 *
 * @return std::optional<std::string> configuration directory path
 */
inline std::optional<std::filesystem::path> GetConfigDir() {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("APPDATA");
#elif defined(__unix__) || defined(__linux__)
  if (auto dir = GetEnv("XDG_CONFIG_HOME"); dir) {
    return dir;
  }
  if (auto dir = GetEnv("HOME"); dir) {
    return fs::path{dir.value()} / ".config";
  }
  return {};
#elif defined(TARGET_OS_MAC)
  if (auto dir = GetEnv("HOME"); dir) {
    return fs::path{dir.value()} / "Library/Application Support";
  }
  return {};
#endif
}

/**
 * @brief Get the cache directory
 *
 * @return std::optional<std::string> Cache directory path
 */
inline std::optional<std::string> GetCacheDir() {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("LOCALAPPDATA");
#elif defined(__unix__) || defined(__linux__)
  if (auto dir = GetEnv("XDG_CACHE_HOME"); dir) {
    return dir;
  }
  if (auto dir = GetEnv("HOME"); dir) {
    fs::path dirpath{dir.value()};
    dirpath /= ".cache";
    return dirpath.string();
  }
  return {};
#elif defined(TARGET_OS_MAC)
  if (auto dir = GetEnv("HOME"); dir) {
    fs::path dirpath{dir.value()};
    dirpath /= "Library/Caches";
    return dirpath.string();
  }
  return {};
#endif
}

/**
 * @brief Get the home directory
 *
 * @return std::optional<std::string> Home Directory path
 */
inline std::optional<std::string> GetHomeDir() {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("USERPROFILE");
#else
  return GetEnv("HOME");
#endif
}

inline std::error_code CreateFilePath(const std::filesystem::path& filepath) {
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
  std::ofstream out{filepath};
  if (!out) {
    return std::make_error_code(static_cast<std::errc>(errno));
  }
  return err;
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_DIRECTORIES_HPP_
