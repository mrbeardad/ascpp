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
      : _org_name{std::move(org_name)},
        _app_name{std::move(app_name)},
        _app_intro{std::move(app_desc)},
        _major_ver(major_ver),
        _minor_ver(minor_ver),
        _patch_ver(patch_ver) {}

  app_info(app_info&&) = default;
  app_info(const app_info&) = default;
  auto operator=(app_info&&) -> app_info& = default;
  auto operator=(const app_info&) -> app_info& = default;
  ~app_info() = default;

  auto org_name() const -> const std::string& { return _org_name; }

  auto app_name() const -> const std::string& { return _app_name; }

  auto app_intro() const -> const std::string& { return _app_intro; }

  auto app_version() const -> std::string {
    return std::format("{}.{}.{}", _major_ver, _minor_ver, _patch_ver);
  }

  auto major_version() const -> int { return _major_ver; }

  auto minor_version() const -> int { return _minor_ver; }

  auto patch_version() const -> int { return _patch_ver; }

 private:
  std::string _org_name;
  std::string _app_name;
  std::string _app_intro;
  int _major_ver;
  int _minor_ver;
  int _patch_ver;
};

}  // namespace ascpp
