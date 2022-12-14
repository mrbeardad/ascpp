#pragma once

#include <format>
#include <string>

namespace ascpp {

class app_info {
 public:
  app_info() = default;

  app_info(std::string org_name,
           std::string app_name,
           std::string app_desc,
           int major_ver,
           int minor_ver,
           int patch_ver)
      : org_name_{std::move(org_name)},
        app_name_{std::move(app_name)},
        app_intro_{std::move(app_desc)},
        major_ver_(major_ver),
        minor_ver_(minor_ver),
        patch_ver_(patch_ver) {}

  app_info(app_info&&) = default;
  app_info(const app_info&) = default;
  auto operator=(app_info&&) -> app_info& = default;
  auto operator=(const app_info&) -> app_info& = default;
  ~app_info() = default;

  auto org_name() const -> const std::string& { return org_name_; }

  auto app_name() const -> const std::string& { return app_name_; }

  auto app_intro() const -> const std::string& { return app_intro_; }

  auto app_version() const -> std::string {
    return std::format("{}.{}.{}", major_ver_, minor_ver_, patch_ver_);
  }

  auto major_version() const -> int { return major_ver_; }

  auto minor_version() const -> int { return minor_ver_; }

  auto patch_version() const -> int { return patch_ver_; }

 private:
  std::string org_name_;
  std::string app_name_;
  std::string app_intro_;
  int major_ver_;
  int minor_ver_;
  int patch_ver_;
};

}  // namespace ascpp
