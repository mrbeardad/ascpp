#ifndef ASCPP_UTILS_SYSTEM_HPP_
#define ASCPP_UTILS_SYSTEM_HPP_

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>

#include "utils/error.hpp"
#include "utils/misc.hpp"
#include "utils/result.hpp"

namespace ascpp {

class SystemError : public Error<SystemError> {
 public:
  enum ErrorCode { kNoError, kGetEnvError, kGetEnvWithEmptyVale, kSetEnvError };

  SystemError() = default;
  ~SystemError() override = default;

  auto name() const noexcept -> const char* override { return "ascpp:system"; }

  auto message(int ec) const -> std::string override {
    switch (ec) {
      case kGetEnvError:
        return "get env error";
      case kGetEnvWithEmptyVale:
        return "env value is empty";
      case kSetEnvError:
        return "set env error";
      default:
        return "unkown error";
    }
  }
};

inline auto make_error_code(SystemError::ErrorCode ec) -> std::error_code {
  return {ec, SystemError::GetInstance()};
}

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

inline auto GetHomeDir() -> Result<std::filesystem::path> {
#if defined(_WIN32) || defined(_WIN64)
  return GetEnv("USERPROFILE");
#else
  return GetEnv("HOME");
#endif
}

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
    return fs::path{dir.Unwrap()} / ".config";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = GetEnv("HOME");
  if (dir.IsOk()) {
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
  if (dir.IsOk()) {
    return dir;
  }
  dir = GetEnv("HOME");
  if (dir.IsOk()) {
    return fs::path{dir.Unwrap()} / ".cache";
  }
  return dir;
#elif defined(TARGET_OS_MAC)
  auto dir = GetEnv("HOME");
  if (dir.IsOk()) {
    return fs::path{dir.Unwrap()} / "Library/Caches";
  }
  return dir;
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

#endif  // !ASCPP_UTILS_SYSTEM_HPP_
