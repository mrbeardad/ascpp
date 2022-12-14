#pragma once

#include <cctype>
#include <chrono>
#include <concepts>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

using std::string_literals::operator""s;        // NOLINT
using std::string_view_literals::operator""sv;  // NOLINT
using std::chrono_literals::operator""ns;       // NOLINT
using std::chrono_literals::operator""us;       // NOLINT
using std::chrono_literals::operator""ms;       // NOLINT
using std::chrono_literals::operator""s;        // NOLINT
using std::chrono_literals::operator""min;      // NOLINT
using std::chrono_literals::operator""h;        // NOLINT

namespace ascpp {

template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

template <template <typename...> class Ref, typename... Args>
constexpr bool is_specialization_v = is_specialization<Ref<Args...>, Ref>::value;

namespace detail {

inline auto get_int_str_and_base(std::string_view str) -> std::pair<std::string, int> {
  auto int_str = ""s;
  auto base = 10;

  if (str.starts_with("-") || str.starts_with("+")) {
    int_str += str[0];
    str.remove_prefix(1);
  }

  if (str.starts_with("0b") || str.starts_with("0B")) {
    if (str.size() <= 2) {
      throw std::invalid_argument("");
    }
    int_str += str.substr(2);
    base = 2;
  } else if (str.starts_with("0o") || str.starts_with("0O")) {
    if (str.size() <= 2) {
      throw std::invalid_argument("");
    }
    int_str += str.substr(2);
    base = 8;
  } else if (str.starts_with("0x") || str.starts_with("0X")) {
    if (str.size() <= 2) {
      throw std::invalid_argument("");
    }
    int_str += str.substr(2);
    base = 16;
  } else if (str.starts_with("0")) {
    int_str += str;
    base = 8;
  } else {
    int_str += str;
    base = 10;
  }

  return {int_str, base};
}

}  // namespace detail

inline auto to_int(std::string_view str) -> int {
  auto [int_str, base] = detail::get_int_str_and_base(str);
  auto idx = 0UZ;
  auto ret = std::stoi(int_str, &idx, base);
  if (idx != int_str.size()) {
    throw std::invalid_argument("");
  }
  return ret;
}

inline auto to_ull(std::string_view str) -> unsigned long long {
  auto [int_str, base] = detail::get_int_str_and_base(str);
  auto idx = 0UZ;
  auto ret = std::stoull(int_str, &idx, base);
  if (idx != int_str.size()) {
    throw std::invalid_argument("");
  }
  return ret;
}

inline auto to_float(const std::string& str) -> float {
  auto idx = 0UZ;
  auto ret = std::stof(str, &idx);
  if (idx != str.size()) {
    throw std::invalid_argument("");
  }
  return ret;
}

inline auto to_double(const std::string& str) -> double {
  auto idx = 0UZ;
  auto ret = std::stod(str, &idx);
  if (idx != str.size()) {
    throw std::invalid_argument("");
  }
  return ret;
}

inline auto debug_string_ansi_graph(std::string_view str) -> std::string {
  auto ret = ""s;
  for (char c : str) {
    if (std::isgraph(static_cast<unsigned char>(c))) {
      ret += c;
    } else {
      ret += std::format("\\x{:02x}", c);
    }
  }
  return ret;
}

}  // namespace ascpp
