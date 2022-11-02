#pragma once

#include <string>

namespace ascpp {

class AppInfo {
 public:
  AppInfo() = default;

  AppInfo(std::string org_name, std::string app_name, std::string app_desc, std::string app_version)
      : org_name_{std::move(org_name)},
        app_name_{std::move(app_name)},
        app_desc_{std::move(app_desc)},
        app_version_{std::move(app_version)} {}

  AppInfo(AppInfo&&) = default;
  AppInfo(const AppInfo&) = default;
  auto operator=(AppInfo&&) -> AppInfo& = default;
  auto operator=(const AppInfo&) -> AppInfo& = default;
  ~AppInfo() = default;

  auto OrgName() const -> const std::string& { return org_name_; }

  auto AppName() const -> const std::string& { return org_name_; }

  auto AppDescription() const -> const std::string& { return org_name_; }

  auto AppVersion() const -> const std::string& { return org_name_; }

 private:
  std::string org_name_;
  std::string app_name_;
  std::string app_desc_;
  std::string app_version_;
};

}  // namespace ascpp
