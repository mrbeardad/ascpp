#pragma once

#include <algorithm>
#include <optional>
#include <string>

#include "app/info.hpp"
#include "utils/cmdline.hpp"
#include "utils/error.hpp"

// TODO: detect single app
namespace ascpp {

struct AppConfig {
  std::string org_name;
  std::string app_name;
  std::string app_desc;
  int major_ver;
  int minor_ver;
  int patch_ver;
};

class App : public AppInfo, public Cmdline {
 public:
  explicit App(AppConfig config)
      : AppInfo(std::move(config.org_name),
                std::move(config.app_name),
                std::move(config.app_desc),
                config.major_ver,
                config.minor_ver,
                config.patch_ver),
        Cmdline{static_cast<AppInfo*>(this)} {}

  App(App&&) = default;
  App(const App&) = default;
  auto operator=(App&&) -> App& = default;
  auto operator=(const App&) -> App& = default;
  ~App() = default;
};

}  // namespace ascpp
