#pragma once

#include <string>

#include "fmt/format.h"

namespace ascpp {

class AppInfo {
 public:
  AppInfo() = default;

  AppInfo(std::string org_name,
          std::string app_name,
          std::string app_desc,
          int major_ver,
          int minor_ver,
          int patch_ver)
      : org_name_{std::move(org_name)},
        app_name_{std::move(app_name)},
        app_desc_{std::move(app_desc)},
        major_ver_(major_ver),
        minor_ver_(minor_ver),
        patch_ver_(patch_ver) {}

  AppInfo(AppInfo&&) = default;
  AppInfo(const AppInfo&) = default;
  auto operator=(AppInfo&&) -> AppInfo& = default;
  auto operator=(const AppInfo&) -> AppInfo& = default;
  ~AppInfo() = default;

  auto OrgName() const -> const std::string& { return org_name_; }

  auto AppName() const -> const std::string& { return app_name_; }

  auto AppDescription() const -> const std::string& { return app_desc_; }

  auto AppVersion() const -> std::string {
    return fmt::format("{}.{}.{}", major_ver_, minor_ver_, patch_ver_);
  }

  auto AppMajorVersion() const -> int { return major_ver_; }

  auto AppMinorVersion() const -> int { return minor_ver_; }

  auto AppPatchVersion() const -> int { return patch_ver_; }

 private:
  std::string org_name_;
  std::string app_name_;
  std::string app_desc_;
  std::string app_usage_;
  int major_ver_;
  int minor_ver_;
  int patch_ver_;
};

}  // namespace ascpp
