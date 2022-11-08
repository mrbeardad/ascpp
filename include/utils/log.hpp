#pragma once

#include <vcruntime.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>

#include "app/info.hpp"
#include "fmt/core.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "utils/system.hpp"

namespace ascpp {

class Logger {
 public:
  enum Level {
    kTrace = spdlog::level::trace,
    kDebug = spdlog::level::debug,
    kInfo = spdlog::level::info,
    kWarn = spdlog::level::warn,
    kError = spdlog::level::err,
    kFatal = spdlog::level::critical
  };

  enum Sink { kFile, kStdout, kStderr };

  struct Config {
    int log_to = kFile;
    int max_file_size = 1024 * 1024 * 10;
    int max_files_count = 3;
    std::string file_path;
  };

  Logger(const AppInfo& info, const Config& cfg) {
    switch (cfg.log_to) {
      case kFile: {
        auto filepath = fs::path{};
        if (cfg.file_path.empty()) {
          filepath = GetCacheDir().Unwrap();
          filepath /= info.OrgName();
          filepath /= info.AppName();
          filepath /= info.AppName() + ".log";
        } else {
          filepath = cfg.file_path;
        }
        CreateFilePath(filepath).Unwrap();
        if (cfg.max_file_size == 0) {
          logger_ = spdlog::basic_logger_st("ascpp_basic_logger", filepath.string());
        } else {
          logger_ = spdlog::rotating_logger_st("ascpp_rotating_logger", filepath.string(),
                                               cfg.max_file_size, cfg.max_files_count);
        }
      }
      case kStdout:
        // TODO: Detected whether stdout is console or redirected to a file
        logger_ = spdlog::stdout_color_st("ascpp_stdout_color");
      case kStderr:
        logger_ = spdlog::stderr_color_st("ascpp_stderr_color");
    }
#ifdef NDEBUG
    SetLevel(kInfo);
#else
    SetLevel(kTrace);
#endif  // NDEBUG
  }

  Logger(Logger&&) = default;
  Logger(const Logger&) = default;
  auto operator=(Logger&&) -> Logger& = default;
  auto operator=(const Logger&) -> Logger& = default;
  ~Logger() = default;

  auto LogTrace(const std::string& msg) -> void { logger_->trace(msg); }

  template <typename... Args>
  auto LogTrace(fmt::format_string<Args...> fmt, Args&&... args) -> void {
    logger_->trace(fmt, std::forward<Args>(args)...);
  }

  auto LogDebug(const std::string& msg) -> void { logger_->debug(msg); }

  template <typename... Args>
  auto LogDebug(fmt::format_string<Args...> fmt, Args&&... args) -> void {
    logger_->debug(fmt, std::forward<Args>(args)...);
  }

  auto LogInfo(const std::string& msg) -> void { logger_->info(msg); }

  template <typename... Args>
  auto LogInfo(fmt::format_string<Args...> fmt, Args&&... args) -> void {
    logger_->info(fmt, std::forward<Args>(args)...);
  }

  auto LogWarn(const std::string& msg) -> void { logger_->warn(msg); }

  template <typename... Args>
  auto LogWarn(fmt::format_string<Args...> fmt, Args&&... args) -> void {
    logger_->warn(fmt, std::forward<Args>(args)...);
  }

  auto LogError(const std::string& msg) -> void { logger_->error(msg); }

  template <typename... Args>
  auto LogError(fmt::format_string<Args...> fmt, Args&&... args) -> void {
    logger_->error(fmt, std::forward<Args>(args)...);
  }

  auto LogFatal(const std::string& msg) -> void { logger_->critical(msg); }

  template <typename... Args>
  auto LogFatal(fmt::format_string<Args...> fmt, Args&&... args) -> void {
    logger_->critical(fmt, std::forward<Args>(args)...);
  }

  auto SetLevel(Level level) -> void {
    logger_->set_level(static_cast<spdlog::level::level_enum>(level));
  }

 private:
  std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace ascpp
