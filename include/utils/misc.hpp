#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <concepts>
#include <cwchar>
#include <expected>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "utils/error.hpp"
#include "utils/utf.hpp"

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

inline auto display_width(char32_t ucs) -> result<size_t> {
  // sorted list of non-overlapping intervals of non-spacing characters
  // generated by "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c"
  constexpr auto combining = std::to_array<std::pair<int, int>>(
      {{0x0300, 0x036F},   {0x0483, 0x0486},   {0x0488, 0x0489},   {0x0591, 0x05BD},
       {0x05BF, 0x05BF},   {0x05C1, 0x05C2},   {0x05C4, 0x05C5},   {0x05C7, 0x05C7},
       {0x0600, 0x0603},   {0x0610, 0x0615},   {0x064B, 0x065E},   {0x0670, 0x0670},
       {0x06D6, 0x06E4},   {0x06E7, 0x06E8},   {0x06EA, 0x06ED},   {0x070F, 0x070F},
       {0x0711, 0x0711},   {0x0730, 0x074A},   {0x07A6, 0x07B0},   {0x07EB, 0x07F3},
       {0x0901, 0x0902},   {0x093C, 0x093C},   {0x0941, 0x0948},   {0x094D, 0x094D},
       {0x0951, 0x0954},   {0x0962, 0x0963},   {0x0981, 0x0981},   {0x09BC, 0x09BC},
       {0x09C1, 0x09C4},   {0x09CD, 0x09CD},   {0x09E2, 0x09E3},   {0x0A01, 0x0A02},
       {0x0A3C, 0x0A3C},   {0x0A41, 0x0A42},   {0x0A47, 0x0A48},   {0x0A4B, 0x0A4D},
       {0x0A70, 0x0A71},   {0x0A81, 0x0A82},   {0x0ABC, 0x0ABC},   {0x0AC1, 0x0AC5},
       {0x0AC7, 0x0AC8},   {0x0ACD, 0x0ACD},   {0x0AE2, 0x0AE3},   {0x0B01, 0x0B01},
       {0x0B3C, 0x0B3C},   {0x0B3F, 0x0B3F},   {0x0B41, 0x0B43},   {0x0B4D, 0x0B4D},
       {0x0B56, 0x0B56},   {0x0B82, 0x0B82},   {0x0BC0, 0x0BC0},   {0x0BCD, 0x0BCD},
       {0x0C3E, 0x0C40},   {0x0C46, 0x0C48},   {0x0C4A, 0x0C4D},   {0x0C55, 0x0C56},
       {0x0CBC, 0x0CBC},   {0x0CBF, 0x0CBF},   {0x0CC6, 0x0CC6},   {0x0CCC, 0x0CCD},
       {0x0CE2, 0x0CE3},   {0x0D41, 0x0D43},   {0x0D4D, 0x0D4D},   {0x0DCA, 0x0DCA},
       {0x0DD2, 0x0DD4},   {0x0DD6, 0x0DD6},   {0x0E31, 0x0E31},   {0x0E34, 0x0E3A},
       {0x0E47, 0x0E4E},   {0x0EB1, 0x0EB1},   {0x0EB4, 0x0EB9},   {0x0EBB, 0x0EBC},
       {0x0EC8, 0x0ECD},   {0x0F18, 0x0F19},   {0x0F35, 0x0F35},   {0x0F37, 0x0F37},
       {0x0F39, 0x0F39},   {0x0F71, 0x0F7E},   {0x0F80, 0x0F84},   {0x0F86, 0x0F87},
       {0x0F90, 0x0F97},   {0x0F99, 0x0FBC},   {0x0FC6, 0x0FC6},   {0x102D, 0x1030},
       {0x1032, 0x1032},   {0x1036, 0x1037},   {0x1039, 0x1039},   {0x1058, 0x1059},
       {0x1160, 0x11FF},   {0x135F, 0x135F},   {0x1712, 0x1714},   {0x1732, 0x1734},
       {0x1752, 0x1753},   {0x1772, 0x1773},   {0x17B4, 0x17B5},   {0x17B7, 0x17BD},
       {0x17C6, 0x17C6},   {0x17C9, 0x17D3},   {0x17DD, 0x17DD},   {0x180B, 0x180D},
       {0x18A9, 0x18A9},   {0x1920, 0x1922},   {0x1927, 0x1928},   {0x1932, 0x1932},
       {0x1939, 0x193B},   {0x1A17, 0x1A18},   {0x1B00, 0x1B03},   {0x1B34, 0x1B34},
       {0x1B36, 0x1B3A},   {0x1B3C, 0x1B3C},   {0x1B42, 0x1B42},   {0x1B6B, 0x1B73},
       {0x1DC0, 0x1DCA},   {0x1DFE, 0x1DFF},   {0x200B, 0x200F},   {0x202A, 0x202E},
       {0x2060, 0x2063},   {0x206A, 0x206F},   {0x20D0, 0x20EF},   {0x302A, 0x302F},
       {0x3099, 0x309A},   {0xA806, 0xA806},   {0xA80B, 0xA80B},   {0xA825, 0xA826},
       {0xFB1E, 0xFB1E},   {0xFE00, 0xFE0F},   {0xFE20, 0xFE23},   {0xFEFF, 0xFEFF},
       {0xFFF9, 0xFFFB},   {0x10A01, 0x10A03}, {0x10A05, 0x10A06}, {0x10A0C, 0x10A0F},
       {0x10A38, 0x10A3A}, {0x10A3F, 0x10A3F}, {0x1D167, 0x1D169}, {0x1D173, 0x1D182},
       {0x1D185, 0x1D18B}, {0x1D1AA, 0x1D1AD}, {0x1D242, 0x1D244}, {0xE0001, 0xE0001},
       {0xE0020, 0xE007F}, {0xE0100, 0xE01EF}});

  auto bsearch = [ucs, &combining]() {
    auto u = static_cast<int>(ucs);
    auto max = static_cast<int>(combining.size() - 1);
    auto min = 0;
    auto mid = 0;

    if (u < combining[0].first || u > combining[max].second)
      return 0;
    while (max >= min) {
      mid = (min + max) / 2;
      if (u > combining[mid].second) {
        min = mid + 1;
      } else if (u < combining[mid].first) {
        max = mid - 1;
      } else {
        return 1;
      }
    }
    return 0;
  };

  // test for 8-bit control characters
  if (ucs == 0) {
    return 0;
  }
  if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0)) {
    return make_error_code(error::INVALID_ARGUMENT);
  }

  // binary search in table of non-spacing characters
  if (bsearch()) {
    return 0;
  }

  /* if we arrive here, ucs is not a combining or C0/C1 control character */
  return 1
         + (ucs >= 0x1100
            && (ucs <= 0x115f ||  // Hangul Jamo init. consonants
                ucs == 0x2329 ||  // LEFT-POINTING ANGLE BRACKET
                ucs == 0x232a ||  // RIGHT-POINTING ANGLE BRACKET
                // CJK ... Yi except IDEOGRAPHIC HALF FILL SPACE:
                (ucs >= 0x2e80 && ucs <= 0xa4cf && ucs != 0x303f)
                || (ucs >= 0xac00 && ucs <= 0xd7a3) ||  // Hangul Syllables
                (ucs >= 0xf900 && ucs <= 0xfaff) ||     // CJK Compatibility Ideographs
                (ucs >= 0xfe10 && ucs <= 0xfe19) ||     // Vertical Forms
                (ucs >= 0xfe30 && ucs <= 0xfe6f) ||     // CJK Compatibility Forms
                (ucs >= 0xff00 && ucs <= 0xff60) ||     // Fullwidth Forms
                (ucs >= 0xffe0 && ucs <= 0xffe6) ||     // Fullwidth Forms
                (ucs >= 0x20000 && ucs <= 0x2fffd) ||   // CJK
                (ucs >= 0x30000 && ucs <= 0x3fffd) ||
                // Miscellaneous Symbols and Pictographs + Emoticons:
                (ucs >= 0x1f300 && ucs <= 0x1f64f) ||
                // Supplemental Symbols and Pictographs:
                (ucs >= 0x1f900 && ucs <= 0x1f9ff)));
}

inline auto display_width(std::u32string_view str) -> result<size_t> {
  auto ret = 0UZ;
  for (auto u : str) {
    auto width = display_width(u);
    if (!width) {
      return width;
    }
    ret += *width;
  }
  return ret;
}

inline auto display_width(std::string_view str) -> result<size_t> {
  auto ustr = utf_conv<char32_t>(str);
  if (!ustr) {
    return ustr.error();
  }
  return display_width(*ustr);
}

inline auto display_width(std::wstring_view str) -> result<size_t> {
  auto ustr = utf_conv<char32_t>(str);
  if (!ustr) {
    return ustr.error();
  }
  return display_width(*ustr);
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

// NOLINTBEGIN(google-runtime-int)

template <typename T>
  requires(std::is_same_v<T, int> || std::is_same_v<T, long> || std::is_same_v<T, long long>
           || std::is_same_v<T, unsigned long> || std::is_same_v<T, unsigned long long>
           || std::is_same_v<T, float> || std::is_same_v<T, double>
           || std::is_same_v<T, long double>)
auto to_number(std::string_view str) -> result<T> try {
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
    auto flt_str = std::string(str);
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

template <typename T>
  requires(std::is_same_v<T, int> || std::is_same_v<T, long> || std::is_same_v<T, long long>
           || std::is_same_v<T, unsigned long> || std::is_same_v<T, unsigned long long>
           || std::is_same_v<T, float> || std::is_same_v<T, double>
           || std::is_same_v<T, long double>)
auto to_number(std::wstring_view str) -> result<T> try {
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
    auto flt_str = std::wstring(str);
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

// NOLINTEND(google-runtime-int)

}  // namespace ascpp
