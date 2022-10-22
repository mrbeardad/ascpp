#ifndef ASCPP_STORAGE_CONFIG_H_
#define ASCPP_STORAGE_CONFIG_H_

#include <chrono>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <shared_mutex>
#include <string>
#include <system_error>

#include "fmt/chrono.h"
#include "nlohmann/json.hpp"

#include "async/app.hpp"
#include "nlohmann/json_fwd.hpp"
#include "utils/result.hpp"
#include "utils/system.hpp"

namespace ascpp {

class Config {
 public:
  enum ConfigType { kUserConfig = 1, kMachineConfig };

  // TODO: Notify confliction, modified obverser
  enum SyncOption { kForceRead = 1, kForceWrite };

  auto Init(ConfigType cfg_type) -> Result<void> {
    auto config_dir = TRY_UNWRAP(GetConfigDir()) / app_->GetOrgName();
    auto config_file = app_->GetAppName() + (cfg_type == kUserConfig ? "_user" : "_machine");
    config_path_ = config_dir / config_file;
    config_path_.replace_extension(".json");
    TRY_UNWRAP(CreateFilePath(config_path_));
    TRY_UNWRAP(ReadConfigFromSystem());

    return {};
  }

  explicit Config(App* app) : app_{app} {}

  ~Config() = default;

  auto ReadConfigFromSystem() -> Result<int> {
    std::unique_lock lock{mutex_};
    std::error_code errc{};

    auto last_write_timestamp = fs::last_write_time(config_path_, errc);
    if (errc) {
      return errc;
    }
    if (last_write_timestamp > last_sync_timestamp_) {
      std::ifstream in{config_path_};
      std::cerr << __LINE__ << ":" << in.good();
      if (in) {
        last_sync_timestamp_ = last_write_timestamp;
      }
      try {
        in >> config_data_;
      } catch (...) {
      }
      std::cerr << __LINE__ << ":" << in.good();
    }
    return 0;
  }

  auto WriteConfigToSystem() -> Result<void> {
    std::shared_lock lock{mutex_};

    if (last_modified_time_ > last_sync_time_) {
      std::ofstream out{config_path_, std::ios_base::out | std::ios_base::trunc};
      std::cerr << __LINE__ << ":" << out.good();
      out << config_data_ << std::flush;
      if (out) {
        last_sync_time_ = last_modified_time_;
        last_sync_timestamp_ = std::filesystem::last_write_time(config_path_);
      }
      std::cerr << __LINE__ << ":" << out.good();
    }
    return {};
  }

  void SyncConfig(SyncOption option) {
    bool need_write = last_modified_time_ > last_sync_time_;
    bool need_read = std::filesystem::last_write_time(config_path_) > last_sync_timestamp_;
    if (need_write && (!need_read || option == kForceWrite)) {
    }
  }

  auto& operator[](const std::string& key) { return config_data_[key]; }

 private:
  App* app_;
  std::shared_mutex mutex_;
  std::filesystem::path config_path_;
  nlohmann::json config_data_;
  std::chrono::system_clock::time_point last_modified_time_;
  std::chrono::system_clock::time_point last_sync_time_;
  std::filesystem::file_time_type last_sync_timestamp_;
};

}  // namespace ascpp

#endif  // ASCPP_STORAGE_CONFIG_H_
