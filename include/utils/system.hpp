#pragma once

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>

#include "utils/error.hpp"

namespace ascpp {

/**
 * @brief Get the system environment variable by it's name.
 */
inline auto get_env(const std::string& name) -> result<std::string> {
#if defined(_WIN32) || defined(_WIN64)
  auto size = size_t();
  auto err = ::getenv_s(&size, nullptr, 0, name.c_str());
  if (err) {
    return make_error_code(error::get_env_failed);
  }
  if (size == 0) {
    return make_error_code(error::get_empty_env);
  }

  auto value = std::string(size, '\0');
  err = ::getenv_s(&size, value.data(), size, name.c_str());
  if (err) {
    return make_error_code(error::get_env_failed);
  }
  value.resize(size - 1);
  return value;
#else
  auto* value = std::getenv(name.c_str());
  if (value == nullptr) {
    return make_error_code(error::GET_ENV_ERROR);
  }
  if (std::strlen(value) == 0) {
    return make_error_code(error::GET_EMPTY_ENV);
  }
  return value;
#endif
}

/**
 * @brief Set the system environment variable.
 */
inline auto set_env(const std::string& name, const std::string& value) -> result<void> {
#if defined(_WIN32) || defined(_WIN64)
  if (::_putenv_s(name.c_str(), value.c_str())) {
    return make_error_code(error::set_env_failed);
  }
  return {};
#else
  if (::setenv(name.c_str(), value.c_str(), 1)) {
    return make_error_code(error::SET_ENV_ERROR);
  }
  return {};
#endif
}

/**
 * @brief Get the home directory of current user.
 */
inline auto get_home_dir() -> result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return get_env("USERPROFILE");
#else
  return get_env("HOME");
#endif
}

/**
 * @brief Get the configuration directory of current user.
 */
inline auto get_config_dir() -> result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return get_env("APPDATA");
#elif defined(__unix__) || defined(__linux__)
  auto dir = get_env("XDG_CONFIG_HOME");
  if (dir) {
    return dir;
  }
  dir = get_env("HOME");
  if (dir) {
    return std::filesystem::path(dir.Unwrap()) / ".config";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = get_env("HOME");
  if (dir) {
    return std::filesystem::path(dir.Unwrap()) / "Library/Application Support";
  }
  return dir;
#endif
}

/**
 * @brief Get the cache directory of current user.
 */
inline auto get_cache_dir() -> result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return get_env("LOCALAPPDATA");
#elif defined(__unix__) || defined(__linux__)
  auto dir = get_env("XDG_CACHE_HOME");
  if (dir) {
    return dir;
  }
  dir = get_env("HOME");
  if (dir) {
    return std::filesystem::path(dir.Unwrap()) / ".cache";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = get_env("HOME");
  if (dir) {
    return std::filesystem::path(dir.Unwrap()) / "Library/Caches";
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
inline auto create_file_path(const std::filesystem::path& filepath) -> result<void> {
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
