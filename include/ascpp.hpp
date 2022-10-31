#pragma once

#include <algorithm>
#include <optional>
#include <string>

#include "app/info.hpp"
#include "utils/cmdline.hpp"
#include "utils/error.hpp"

namespace ascpp {

inline auto app = std::optional<AppInfo>{};
inline auto cmd = std::optional<Cmdline>{};

inline auto Init(std::string org_name,
                 std::string app_name,
                 std::string app_desc,
                 std::string app_version) -> Result<void> {
  app = AppInfo{std::move(org_name), std::move(app_name), std::move(app_desc),
                std::move(app_version)};
  cmd = Cmdline{&app.value()};
}

}  // namespace ascpp
