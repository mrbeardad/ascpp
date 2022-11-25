#pragma once

#include <cctype>
#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>

#include "fmt/format.h"

namespace ascpp {

using std::string_literals::operator""s;        // NOLINT
using std::string_view_literals::operator""sv;  // NOLINT
using std::chrono_literals::operator""ns;       // NOLINT
using std::chrono_literals::operator""us;       // NOLINT
using std::chrono_literals::operator""ms;       // NOLINT
using std::chrono_literals::operator""s;        // NOLINT
using std::chrono_literals::operator""min;      // NOLINT
using std::chrono_literals::operator""h;        // NOLINT

template <typename T>
using PassT = std::conditional_t<std::is_integral_v<T> || std::is_floating_point_v<T>
                                     || std::is_pointer_v<T>,
                                 T,
                                 const T&>;

inline auto DebugGraphAnsiString(std::string_view str) -> std::string {
  auto ret = ""s;
  for (char c : str) {
    if (std::isgraph(c)) {
      ret += c;
    } else {
      ret += fmt::format("\\x{:02x}", c);
    }
  }
  return ret;
}

}  // namespace ascpp
