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
#include "utils/system.hpp"

namespace ascpp {

class Config {
 public:
  enum ConfigType { kUserConfig = 1, kMachineConfig };

  // TODO: Notify confliction, modified obverser
  enum SyncOption { kForceRead = 1, kForceWrite };

  explicit Config(App* app) : app_{app} {}

  Init : app_{app},
         config_file_{
             GetConfigDir().Unwrap() / app->GetOrgName()
             / (app->GetAppName() + (cfg_type == kUserConfig ? "_user.json" : "_machine.json"))} {
    CreateFilePath(config_file_);
    ReadConfig();
  }

  ~Config() = default;

  void ReadConfig() {
    std::unique_lock lock{mutex_};
    std::error_code errc{};
    auto last_write_timestamp = std::filesystem::last_write_time(config_file_, errc);
    if (errc) {
      return;
    }
    if (last_write_timestamp > last_sync_timestamp_) {
      std::ifstream in{config_file_, std::ios_base::in | std::ios_base::app};
      if (in) {
        last_sync_timestamp_ = last_write_timestamp;
      }
      in >> config_data_;
      std::cerr << in.good();
    }
  }

  void WriteConfig() {
    std::shared_lock lock{mutex_};
    if (last_modified_time_ > last_sync_time_) {
      std::ofstream out{config_file_, std::ios_base::out | std::ios_base::trunc};
      out << config_data_ << std::flush;
      if (out) {
        last_sync_time_ = last_modified_time_;
        last_sync_timestamp_ = std::filesystem::last_write_time(config_file_);
      }
    }
  }

  void SyncConfig(SyncOption option) {
    bool need_write = last_modified_time_ > last_sync_time_;
    bool need_read = std::filesystem::last_write_time(config_file_) > last_sync_timestamp_;
    if (need_write && (!need_read || option == kForceWrite)) {
    }
  }

  auto& operator[](const std::string& key) { return config_data_[key]; }

 private:
  App* app_;
  std::shared_mutex mutex_;
  std::filesystem::path config_file_;
  nlohmann::json config_data_;
  std::chrono::system_clock::time_point last_modified_time_;
  std::chrono::system_clock::time_point last_sync_time_;
  std::filesystem::file_time_type last_sync_timestamp_;
};

}  // namespace ascpp

#endif  // ASCPP_STORAGE_CONFIG_H_
