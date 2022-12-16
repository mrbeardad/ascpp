#pragma once

#include <cctype>
#include <chrono>
#include <codecvt>
#include <concepts>
#include <cwchar>
#include <expected>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "utils/error.hpp"

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

template <typename Test, template <typename...> class Ref>
constexpr bool is_specialization_v = is_specialization<Test, Ref>::value;

template <typename ToCharT, typename FromCharT>
constexpr auto codecvt(std::basic_string_view<FromCharT> from)
    -> result<std::basic_string<ToCharT>> {
  auto cvt = std::codecvt<FromCharT, ToCharT, std::mbstate_t>();
  auto& f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());
  auto mb = std::mbstate_t();
  auto to = std::string(from.size() * f.max_length(), '\0');
  const FromCharT* from_next;
  ToCharT* to_next;
  f.out(mb, from.data(), &from[from.size()], from_next, to.data(), &to[to.size()], to_next);
  if (auto idx = from_next - from.data(); idx != from.size()) {
    return make_error_code(error::CODECVT_FAILED);
  }
  to.resize(to_next - to.data());
  return to;
}

template <typename ToCharT, typename FromCharT>
constexpr auto codecvt(std::basic_string<FromCharT> from) {
  return codecvt<ToCharT>(std::basic_string_view(from));
}

template <typename ToCharT, typename FromCharT>
constexpr auto codecvt(const FromCharT* from) {
  return codecvt<ToCharT>(std::basic_string_view(from));
}

namespace detail {

constexpr auto get_int_str_and_base(std::string_view str)
    -> std::optional<std::pair<std::string, int>> {
  auto int_str = ""s;
  auto base = 10;

  if (str.starts_with("+") || str.starts_with("-")) {
    int_str += str[0];
    str.remove_prefix(1);
  }

  if (str.starts_with("0b") || str.starts_with("0B")) {
    if (str.size() <= 2) {
      return {};
    }
    int_str += str.substr(2);
    base = 2;
  } else if (str.starts_with("0o") || str.starts_with("0O")) {
    if (str.size() <= 2) {
      return {};
    }
    int_str += str.substr(2);
    base = 8;
  } else if (str.starts_with("0x") || str.starts_with("0X")) {
    if (str.size() <= 2) {
      return {};
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

  return std::pair{int_str, base};
}

constexpr auto get_int_str_and_base(std::wstring_view str)
    -> std::optional<std::pair<std::wstring, int>> {
  auto int_str = L""s;
  auto base = 10;

  if (str.starts_with(L"+") || str.starts_with(L"-")) {
    int_str += str[0];
    str.remove_prefix(1);
  }

  if (str.starts_with(L"0b") || str.starts_with(L"0B")) {
    if (str.size() <= 2) {
      return {};
    }
    int_str += str.substr(2);
    base = 2;
  } else if (str.starts_with(L"0o") || str.starts_with(L"0O")) {
    if (str.size() <= 2) {
      return {};
    }
    int_str += str.substr(2);
    base = 8;
  } else if (str.starts_with(L"0x") || str.starts_with(L"0X")) {
    if (str.size() <= 2) {
      return {};
    }
    int_str += str.substr(2);
    base = 16;
  } else if (str.starts_with(L"0")) {
    int_str += str;
    base = 8;
  } else {
    int_str += str;
    base = 10;
  }

  return std::pair{int_str, base};
}

}  // namespace detail

template <typename T, typename CharT>
  requires((std::is_same_v<T, int> || std::is_same_v<T, long> || std::is_same_v<T, long long>
            || std::is_same_v<T, unsigned long> || std::is_same_v<T, unsigned long long>
            || std::is_same_v<T, float> || std::is_same_v<T, double>
            || std::is_same_v<T, long double>)
           && (std::is_same_v<CharT, char> || std::is_same_v<CharT, wchar_t>))
auto to_number(std::basic_string_view<CharT> str) -> result<T> try {
  auto ret = T{};
  auto idx = 0UZ;

  if constexpr (std::is_integral_v<T>) {
    auto res = detail::get_int_str_and_base(str);
    if (!res) {
      return make_error_code(error::INVALID_ARGUMENT);
    }
    auto [int_str, base] = *res;
    if constexpr (std::is_same_v<T, int>) {
      ret = std::stoi(int_str, &idx, base);
    } else if constexpr (std::is_same_v<T, long>) {
      ret = std::stol(int_str, &idx, base);
    } else if constexpr (std::is_same_v<T, long long>) {
      ret = std::stoll(int_str, &idx, base);
    } else if constexpr (std::is_same_v<T, unsigned long>) {
      ret = std::stoul(int_str, &idx, base);
    } else if constexpr (std::is_same_v<T, unsigned long long>) {
      ret = std::stoull(int_str, &idx, base);
    }
    if (idx != int_str.size()) {
      return make_error_code(error::INVALID_ARGUMENT);
    }
  } else if constexpr (std::is_floating_point_v<T>) {
    auto flt_str = std::basic_string<CharT>(str);
    if constexpr (std::is_same_v<T, float>) {
      ret = std::stof(flt_str, &idx);
    } else if constexpr (std::is_same_v<T, double>) {
      ret = std::stod(flt_str, &idx);
    } else if constexpr (std::is_same_v<T, long double>) {
      ret = std::stold(flt_str, &idx);
    }
    if (idx != flt_str.size()) {
      return make_error_code(error::INVALID_ARGUMENT);
    }
  }

  return ret;
} catch (const std::invalid_argument&) {
  return make_error_code(error::INVALID_ARGUMENT);
} catch (const std::out_of_range&) {
  return make_error_code(error::OUT_OF_RANGE);
}

template <typename T, typename CharT>
auto to_number(const std::basic_string<CharT>& str) -> result<T> {
  return to_number<T>(std::basic_string_view<CharT>(str));
}

template <typename T, typename CharT>
auto to_number(const CharT* str) -> result<T> {
  return to_number<T>(std::basic_string_view<CharT>(str));
}

constexpr auto debug_string_ansi_graph(std::string_view str) -> std::string {
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
