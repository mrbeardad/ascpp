#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>

namespace ascpp {

namespace fs = std::filesystem;                 // NOLINT
namespace ch = std::chrono;                     // NOLINT
using std::string_literals::operator""s;        // NOLINT
using std::string_view_literals::operator""sv;  // NOLINT
using std::chrono_literals::operator""ns;       // NOLINT
using std::chrono_literals::operator""us;       // NOLINT
using std::chrono_literals::operator""ms;       // NOLINT
using std::chrono_literals::operator""s;        // NOLINT
using std::chrono_literals::operator""min;      // NOLINT
using std::chrono_literals::operator""h;        // NOLINT

}  // namespace ascpp
