#pragma once

#include <vcruntime.h>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>

#include "utils/error.hpp"

namespace ascpp {

class SysErr : public std::error_category {
 public:
  enum Errc { NO_ERROR, GET_ENV_ERROR, GET_EMPTY_ENV, SET_ENV_ERROR };

  auto name() const noexcept -> const char* override { return "ascpp:system"; }

  auto message(int ec) const -> std::string override {
    switch (ec) {
      case GET_ENV_ERROR:
        return "get env error";
      case GET_EMPTY_ENV:
        return "env value is empty";
      case SET_ENV_ERROR:
        return "set env error";
      default:
        return "unkown error";
    }
  }
};

}  // namespace ascpp

ENABLE_ERROR_CODE(ascpp::SysErr);

namespace ascpp {

/**
 * @brief Get the system environment variable by it's name.
 */
inline auto GetEnv(const std::string& name) -> Result<std::string> {
#if defined(_WIN32) || defined(_WIN64)
  auto size = size_t();
  auto err = ::getenv_s(&size, nullptr, 0, name.c_str());
  if (err) {
    return std::make_error_code(SysErr::GET_ENV_ERROR);
  }
  if (size == 0) {
    return std::make_error_code(SysErr::GET_EMPTY_ENV);
  }

  auto value = std::string(size, '\0');
  err = ::getenv_s(&size, value.data(), size, name.c_str());
  if (err) {
    return std::make_error_code(SysErr::GET_ENV_ERROR);
  }
  value.resize(size - 1);
  return value;
#else
  auto* value = std::getenv(name.c_str());
  if (value == nullptr) {
    return std::make_error_code(SysErr::GET_ENV_ERROR);
  }
  if (std::strlen(value) == 0) {
    return std::make_error_code(SysErr::GET_EMPTY_ENV);
  }
  return value;
#endif
}

/**
 * @brief Set the system environment variable.
 */
inline auto SetEnv(const std::string& name, const std::string& value) -> Result<void> {
#if defined(_WIN32) || defined(_WIN64)
  if (::_putenv_s(name.c_str(), value.c_str())) {
    return std::make_error_code(SysErr::SET_ENV_ERROR);
  }
  return {};
#else
  if (::setenv(name.c_str(), value.c_str(), 1)) {
    return std::make_error_code(SysErr::SET_ENV_ERROR);
  }
  return {};
#endif
}

/**
 * @brief Get the home directory of current user.
 */
inline auto GetHomeDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("USERPROFILE");
#else
  return GetEnv("HOME");
#endif
}

/**
 * @brief Get the configuration directory of current user.
 */
inline auto GetConfigDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("APPDATA");
#elif defined(__unix__) || defined(__linux__)
  auto dir = GetEnv("XDG_CONFIG_HOME");
  if (dir.IsOk()) {
    return dir;
  }
  dir = GetEnv("HOME");
  if (dir.IsOk()) {
    return fs::path(dir.Unwrap()) / ".config";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = GetEnv("HOME");
  if (dir.IsOk()) {
    return fs::path(dir.Unwrap()) / "Library/Application Support";
  }
  return dir;
#endif
}

/**
 * @brief Get the cache directory of current user.
 */
inline auto GetCacheDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("LOCALAPPDATA");
#elif defined(__unix__) || defined(__linux__)
  auto dir = GetEnv("XDG_CACHE_HOME");
  if (dir.IsOk()) {
    return dir;
  }
  dir = GetEnv("HOME");
  if (dir.IsOk()) {
    return fs::path(dir.Unwrap()) / ".cache";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = GetEnv("HOME");
  if (dir.IsOk()) {
    return fs::path(dir.Unwrap()) / "Library/Caches";
  }
  return dir;
#endif
}

/**
 * @brief Create the directories and ordinary file in given path.
 *
 * If it is a ordinary file, create the all directories in it's parent path and then create the
 * empty file. You can create std::path that store the directory path by `path /= ""`.
 */
inline auto CreateFilePath(const std::filesystem::path& filepath) -> Result<void> {
  auto err = std::error_code();
  auto result = std::filesystem::exists(filepath, err);
  if (err) {
    return err;
  }
  if (result) {
    return {};
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
  if (filepath.has_filename()) {
    const std::ofstream out(filepath);
    if (!out) {
      return std::make_error_code(static_cast<std::errc>(errno));
    }
  }
  return {};
}

}  // namespace ascpp
