#pragma once

#include <string>
#include <string_view>

#include "utils/error.hpp"

namespace ascpp {

namespace detail {

using std::string_literals::operator""s;

inline auto to_utf32(std::u8string_view input, size_t* idx = nullptr) -> std::u32string {
  auto ret = U""s;
  auto orig_i = 0UZ;
  for (auto i = 0UZ; i < input.size();) {
    auto ch0 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
    if (ch0 < 0x80) {  // 0xxx_xxxx
      ret += static_cast<char32_t>(ch0);
    } else if (ch0 < 0xC0) {
      break;
    } else if (ch0 < 0xE0) {  // 110x_xxxx 10xx_xxxx
      if (++i >= input.size()) {
        break;
      }
      auto ch1 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch1 >> 6 != 2) {
        break;
      }
      ret += static_cast<char32_t>((ch0 << 6) + ch1 - 0x3080);
    } else if (ch0 < 0xF0) {  // 1110_xxxx 10xx_xxxx 10xx_xxxx
      if (++i >= input.size()) {
        break;
      }
      auto ch1 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch1 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch2 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch2 >> 6 != 2) {
        break;
      }
      ret += static_cast<char32_t>((ch0 << 12) + (ch1 << 6) + ch2 - 0xE2080);
    } else if (ch0 < 0xF8) {  // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      if (++i >= input.size()) {
        break;
      }
      auto ch1 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch1 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch2 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch2 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch3 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch3 >> 6 != 2) {
        break;
      }
      ret += static_cast<char32_t>((ch0 << 18) + (ch1 << 12) + (ch2 << 6) + ch3 - 0x3C82080);
    } else if (ch0 < 0xFC) {  // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      if (++i >= input.size()) {
        break;
      }
      auto ch1 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch1 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch2 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch2 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch3 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch3 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch4 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch4 >> 6 != 2) {
        break;
      }
      ret += static_cast<char32_t>((ch0 << 24) + (ch1 << 18) + (ch2 << 12) + (ch3 << 6) + ch4
                                   - 0xFA082080);
    } else if (ch0 < 0xFE) {  // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      if (++i >= input.size()) {
        break;
      }
      auto ch1 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch1 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch2 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch2 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch3 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch3 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch4 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch4 >> 6 != 2) {
        break;
      }
      if (++i >= input.size()) {
        break;
      }
      auto ch5 = static_cast<uint32_t>(static_cast<uint8_t>(input[i]));
      if (ch5 >> 6 != 2) {
        break;
      }
      ret += static_cast<char32_t>((ch0 << 30) + (ch1 << 24) + (ch2 << 18) + (ch3 << 12)
                                   + (ch4 << 6) + ch5 - 0x82082080);
    } else {
      break;
    }
    orig_i = ++i;
  }
  if (idx) {
    *idx = orig_i;
  }
  return ret;
}

inline auto to_utf32(std::u16string_view input, size_t* idx = nullptr) -> std::u32string {
  auto ret = U""s;
  auto orig_i = 0UZ;
  for (auto i = 0UZ; i < input.size();) {
    auto ch0 = static_cast<uint32_t>(static_cast<uint16_t>(input[i]));
    if (ch0 < 0xD800) {  // [0x0000‥0xD7FF]
      ret += static_cast<char32_t>(ch0);
    } else if (ch0 < 0xDC00) {  // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
      if (++i >= input.size()) {
        break;
      }
      auto ch1 = static_cast<uint32_t>(static_cast<uint16_t>(input[i]));
      if (ch1 >> 10 != 0x37) {
        break;
      }
      ret += static_cast<char32_t>((ch0 << 10) + ch1 - 0x35FDC00);
    } else if (ch0 < 0xE000) {
      break;
    } else {  // [0xE000‥0xFFFF]
      ret += static_cast<char32_t>(ch0);
    }
    orig_i = ++i;
  }
  if (idx) {
    *idx = orig_i;
  }
  return ret;
}

inline auto to_utf32(std::u32string_view input, size_t* idx = nullptr) -> std::u32string {
  auto ret = U""s;
  auto orig_i = 0UZ;
  for (auto i = 0UZ; i < input.size();) {
    if (static_cast<uint32_t>(input[i]) < 0x80000000) {
      ret += input[i];
    } else {
      break;
    }
    orig_i = ++i;
  }
  if (idx) {
    *idx = orig_i;
  }
  return ret;
}

inline auto to_utf32(std::string_view input, size_t* idx = nullptr) -> std::u32string {
  auto begin = reinterpret_cast<const char8_t*>(input.data());
  return to_utf32(std::u8string_view(begin, begin + input.size()), idx);
}

#if defined(_WIN32)
inline auto to_utf32(std::wstring_view input, size_t* idx = nullptr) -> std::u32string {
  auto begin = reinterpret_cast<const char16_t*>(input.data());
  return to_utf32(std::u16string_view(begin, begin + input.size()), idx);
}
#elif defined(__linux__) || defined(TARGET_OS_MAC)
inline auto to_utf32(std::wstring_view input, size_t* idx = nullptr) -> std::u32string {
  auto begin = reinterpret_cast<const char32_t*>(input.data());
  return to_utf32(std::u32string_view(begin, begin + input.size()), idx);
}
#endif

template <typename ToCharT>
auto from_utf32(std::u32string_view input, size_t* idx = nullptr) -> std::basic_string<ToCharT>;

template <>
inline auto from_utf32<char8_t>(std::u32string_view input, size_t* idx) -> std::u8string {
  auto ret = u8""s;
  auto i = 0UZ;
  for (; i < input.size(); ++i) {
    auto cp = static_cast<uint32_t>(input[i]);
    if (cp < 0x80) {  // 0xxx_xxxx
      ret += static_cast<char8_t>(cp);
    } else if (cp < 0x800) {  // 110x_xxxx 10xx_xxxx
      ret += (static_cast<char8_t>(0xC0 | cp >> 6));
      goto _1;
    } else if (cp < 0x10000) {  // 1110_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char8_t>(0xE0 | cp >> 12));
      goto _2;
    } else if (cp < 0x200000) {  // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char8_t>(0xF0 | cp >> 18));
      goto _3;
    } else if (cp < 0x4000000) {  // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char8_t>(0xF8 | cp >> 24));
      goto _4;
    } else if (cp < 0x80000000) {  // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char8_t>(0xFC | cp >> 30));
      goto _5;
    } else {
      break;
    }
    continue;
  _5:
    ret += static_cast<char8_t>(0x80 | (cp >> 24 & 0x3F));
  _4:
    ret += static_cast<char8_t>(0x80 | (cp >> 18 & 0x3F));
  _3:
    ret += static_cast<char8_t>(0x80 | (cp >> 12 & 0x3F));
  _2:
    ret += static_cast<char8_t>(0x80 | (cp >> 6 & 0x3F));
  _1:
    ret += static_cast<char8_t>(0x80 | (cp & 0x3F));
  }
  if (idx) {
    *idx = i;
  }
  return ret;
}

template <>
inline auto from_utf32<char16_t>(std::u32string_view input, size_t* idx) -> std::u16string {
  auto ret = u""s;
  auto i = 0UZ;
  for (; i < input.size(); ++i) {
    auto cp = static_cast<uint32_t>(input[i]);
    if (cp < 0xD800) {
      // [0x0000‥0xD7FF]
      ret += (static_cast<char16_t>(cp));
    } else if (cp < 0x10000) {
      if (cp < 0xE000) {
        break;
      }
      // [0xE000‥0xFFFF]
      ret += static_cast<char16_t>(cp);
    } else if (cp < 0x110000) {
      // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
      ret += static_cast<char16_t>(0xD7C0 + (cp >> 10));
      ret += static_cast<char16_t>(0xDC00 + (cp & 0x3FF));
    } else {
      break;
    }
  }
  if (idx) {
    *idx = i;
  }
  return ret;
}

template <>
inline auto from_utf32<char32_t>(std::u32string_view input, size_t* idx) -> std::u32string {
  auto ret = U""s;
  auto i = 0UZ;
  for (; i < input.size(); ++i) {
    auto cp = static_cast<uint32_t>(input[i]);
    if (cp < 0x80000000) {
      ret += static_cast<char32_t>(cp);
    } else {
      break;
    }
  }
  if (idx) {
    *idx = i;
  }
  return ret;
}

template <>
inline auto from_utf32<char>(std::u32string_view input, size_t* idx) -> std::string {
  auto ret = ""s;
  auto i = 0UZ;
  for (; i < input.size(); ++i) {
    auto cp = static_cast<uint32_t>(input[i]);
    if (cp < 0x80) {  // 0xxx_xxxx
      ret += static_cast<char>(cp);
    } else if (cp < 0x800) {  // 110x_xxxx 10xx_xxxx
      ret += (static_cast<char>(0xC0 | cp >> 6));
      goto _1;
    } else if (cp < 0x10000) {  // 1110_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char>(0xE0 | cp >> 12));
      goto _2;
    } else if (cp < 0x200000) {  // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char>(0xF0 | cp >> 18));
      goto _3;
    } else if (cp < 0x4000000) {  // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char>(0xF8 | cp >> 24));
      goto _4;
    } else if (cp < 0x80000000) {  // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
      ret += (static_cast<char>(0xFC | cp >> 30));
      goto _5;
    } else {
      break;
    }
    continue;
  _5:
    ret += static_cast<char>(0x80 | (cp >> 24 & 0x3F));
  _4:
    ret += static_cast<char>(0x80 | (cp >> 18 & 0x3F));
  _3:
    ret += static_cast<char>(0x80 | (cp >> 12 & 0x3F));
  _2:
    ret += static_cast<char>(0x80 | (cp >> 6 & 0x3F));
  _1:
    ret += static_cast<char>(0x80 | (cp & 0x3F));
  }
  if (idx) {
    *idx = i;
  }
  return ret;
}

#if defined(_WIN32)
template <>
inline auto from_utf32<wchar_t>(std::u32string_view input, size_t* idx) -> std::wstring {
  auto ret = L""s;
  auto i = 0UZ;
  for (; i < input.size(); ++i) {
    auto cp = static_cast<uint32_t>(input[i]);
    if (cp < 0xD800) {
      // [0x0000‥0xD7FF]
      ret += (static_cast<wchar_t>(cp));
    } else if (cp < 0x10000) {
      if (cp < 0xE000) {
        break;
      }
      // [0xE000‥0xFFFF]
      ret += static_cast<wchar_t>(cp);
    } else if (cp < 0x110000) {
      // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
      ret += static_cast<wchar_t>(0xD7C0 + (cp >> 10));
      ret += static_cast<wchar_t>(0xDC00 + (cp & 0x3FF));
    } else {
      break;
    }
  }
  if (idx) {
    *idx = i;
  }
  return ret;
}
#elif defined(__linux__) || defined(TARGET_OS_MAC)
template <>
inline auto from_utf32<wchar_t>(std::u32string_view input, size_t* idx) -> std::wstring {
  auto ret = L""s;
  auto i = 0UZ;
  for (; i < input.size(); ++i) {
    auto cp = static_cast<uint32_t>(input[i]);
    if (cp < 0x80000000) {
      ret += static_cast<wchar_t>(cp);
    } else {
      break;
    }
  }
  if (idx) {
    *idx = i;
  }
  return ret;
}
#endif

}  // namespace detail

template <typename ToCharT, typename FromCharT>
auto utf_conv(std::basic_string_view<FromCharT> input, size_t* idx = nullptr)
    -> result<std::basic_string<ToCharT>> {
  auto index = 0UZ;
  auto runes = detail::to_utf32(input, &index);
  if (idx) {
    *idx = index;
  }
  if (index != input.size()) {
    return make_error_code(error::INVALID_ARGUMENT);
  }
  auto out = detail::from_utf32<ToCharT>(runes, &index);
  if (idx) {
    *idx = index;
  }
  if (index != runes.size()) {
    return make_error_code(error::INVALID_ARGUMENT);
  }
  return out;
}

template <typename ToCharT, typename FromCharT>
auto utf_conv(const std::basic_string<FromCharT>& input, size_t* idx = nullptr)
    -> result<std::basic_string<ToCharT>> {
  return utf_conv<ToCharT>(std::basic_string_view<FromCharT>(input), idx);
}

template <typename ToCharT, typename FromCharT>
auto utf_conv(const FromCharT* input, size_t* idx = nullptr) -> result<std::basic_string<ToCharT>> {
  return utf_conv<ToCharT>(std::basic_string_view<FromCharT>(input), idx);
}

}  // namespace ascpp
