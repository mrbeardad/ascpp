#include "utils/utf.hpp"

#include "gtest/gtest.h"

using std::string_literals::operator""s;

// NOLINTBEGIN(modernize-use-trailing-return-type)

TEST(TestUnicode, ToUtf32) {
  EXPECT_EQ(ascpp::detail::to_utf32("我dnmd🤡"), U"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::to_utf32(L"我dnmd🤡"), U"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::to_utf32(u8"我dnmd🤡"), U"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::to_utf32(u"我dnmd🤡"), U"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::to_utf32(U"我dnmd🤡"), U"我dnmd🤡");

  auto idx = 0UZ;
  ascpp::detail::to_utf32(u8"ascii", &idx);
  EXPECT_EQ(idx, 5);
  ascpp::detail::to_utf32(u8"E\x80", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xC0\x80", &idx);
  EXPECT_EQ(idx, 3);
  ascpp::detail::to_utf32(u8"E\xC0\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xC0", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xE0\x80\x80", &idx);
  EXPECT_EQ(idx, 4);
  ascpp::detail::to_utf32(u8"E\xE0\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xE0\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xE0", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0\x80\x80\x80", &idx);
  EXPECT_EQ(idx, 5);
  ascpp::detail::to_utf32(u8"E\xF0\x80\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0\x80\x80\x80", &idx);
  EXPECT_EQ(idx, 5);
  ascpp::detail::to_utf32(u8"E\xF0\x80\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF0", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF8\x80\x80\x80\x80", &idx);
  EXPECT_EQ(idx, 6);
  ascpp::detail::to_utf32(u8"E\xF8\x80\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF8\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF8\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xF8", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFC\x80\x80\x80\x80\x80", &idx);
  EXPECT_EQ(idx, 7);
  ascpp::detail::to_utf32(u8"E\xFC\x80\x80\x80\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFC\x80\x80\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFC\x80\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFC\x80\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFC\x00", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFC", &idx);
  EXPECT_EQ(idx, 1);
  ascpp::detail::to_utf32(u8"E\xFE", &idx);
  EXPECT_EQ(idx, 1);

  idx = 0UZ;
  auto u = u"E"s;
  u += static_cast<char16_t>(0x8800);
  ascpp::detail::to_utf32(u, &idx);
  EXPECT_EQ(idx, 2);
  u = u"E"s;
  u += static_cast<char16_t>(0xD800);
  u += static_cast<char16_t>(0xDC00);
  ascpp::detail::to_utf32(u, &idx);
  EXPECT_EQ(idx, 3);
  u = u"E"s;
  u += static_cast<char16_t>(0xD800);
  u += static_cast<char16_t>(0x3800);
  ascpp::detail::to_utf32(u, &idx);
  EXPECT_EQ(idx, 1);
  u = u"E"s;
  u += static_cast<char16_t>(0xDC00);
  ascpp::detail::to_utf32(u, &idx);
  EXPECT_EQ(idx, 1);
  u = u"E"s;
  u += static_cast<char16_t>(0xE000);
  ascpp::detail::to_utf32(u, &idx);
  EXPECT_EQ(idx, 2);

  idx = 0UZ;
  auto U = U"E"s;
  U += static_cast<char32_t>(0x7FFFFFFF);
  ascpp::detail::to_utf32(U, &idx);
  EXPECT_EQ(idx, 2);
  U = U"E"s;
  U += static_cast<char32_t>(0x80000000);
  ascpp::detail::to_utf32(U, &idx);
  EXPECT_EQ(idx, 1);
}

TEST(TestUnicode, FromUtf32) {
  EXPECT_EQ(ascpp::detail::from_utf32<char>(U"我dnmd🤡"), "我dnmd🤡");
  EXPECT_EQ(ascpp::detail::from_utf32<wchar_t>(U"我dnmd🤡"), L"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::from_utf32<char8_t>(U"我dnmd🤡"), u8"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::from_utf32<char16_t>(U"我dnmd🤡"), u"我dnmd🤡");
  EXPECT_EQ(ascpp::detail::from_utf32<char32_t>(U"我dnmd🤡"), U"我dnmd🤡");
}

TEST(TestUnicode, UtfCvt) {
  EXPECT_EQ(ascpp::utf_conv<char>("你tnd真是个人才🤡"), "你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char>(L"你tnd真是个人才🤡"), "你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char>(U"你tnd真是个人才🤡"), "你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char>(u"你tnd真是个人才🤡"), "你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<wchar_t>("你tnd真是个人才🤡"), L"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<wchar_t>(L"你tnd真是个人才🤡"), L"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<wchar_t>(U"你tnd真是个人才🤡"), L"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<wchar_t>(u"你tnd真是个人才🤡"), L"你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<char8_t>("你tnd真是个人才🤡"), u8"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char8_t>(L"你tnd真是个人才🤡"), u8"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char8_t>(U"你tnd真是个人才🤡"), u8"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char8_t>(u"你tnd真是个人才🤡"), u8"你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<char16_t>("你tnd真是个人才🤡"), u"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char16_t>(L"你tnd真是个人才🤡"), u"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char16_t>(U"你tnd真是个人才🤡"), u"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char16_t>(u"你tnd真是个人才🤡"), u"你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<char32_t>("你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char32_t>(L"你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char32_t>(U"你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char32_t>(u"你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
}

// NOLINTEND(modernize-use-trailing-return-type)
