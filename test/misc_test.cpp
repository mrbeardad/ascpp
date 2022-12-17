#include "utils/misc.hpp"

#include <cmath>
#include <vector>

#include "gtest/gtest.h"

#include "utils/error.hpp"

// NOLINTBEGIN(modernize-use-trailing-return-type)

TEST(TestMisc, IsSpecialization) {
  auto b = ascpp::is_specialization<std::vector<int>, std::vector>::value;
  EXPECT_TRUE(b);
  b = ascpp::is_specialization_v<std::vector<int>, std::vector>;
  EXPECT_TRUE(b);
}

TEST(TestMisc, UtfCvt) {
  EXPECT_EQ(ascpp::utf_conv<char>("你tnd真是个人才🤡"), "你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char>(L"你tnd真是个人才🤡"), "你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char>(U"你tnd真是个人才🤡"), "你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char>(u"你tnd真是个人才🤡"), "你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<wchar_t>("你tnd真是个人才🤡"), L"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<wchar_t>(L"你tnd真是个人才🤡"), L"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<wchar_t>(U"你tnd真是个人才🤡"), L"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<wchar_t>(u"你tnd真是个人才🤡"), L"你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<char16_t>("你tnd真是个人才🤡"), u"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char16_t>(L"你tnd真是个人才🤡"), u"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char16_t>(U"你tnd真是个人才🤡"), u"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char16_t>(u"你tnd真是个人才🤡"), u"你tnd真是个人才🤡");

  EXPECT_EQ(ascpp::utf_conv<char32_t>("你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char32_t>(L"你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char32_t>(U"你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
  EXPECT_EQ(ascpp::utf_conv<char32_t>(u"你tnd真是个人才🤡"), U"你tnd真是个人才🤡");
}

TEST(TestMisc, DisplayWidth) {
  EXPECT_EQ(ascpp::display_width(U'\0'), 0);
  EXPECT_EQ(ascpp::display_width(U'a'), 1);
  EXPECT_EQ(ascpp::display_width(U'你'), 2);
  EXPECT_EQ(ascpp::display_width(U'🤡'), 2);
  EXPECT_FALSE(ascpp::display_width(U'\t'));
  EXPECT_FALSE(ascpp::display_width(U'\n'));
  EXPECT_FALSE(ascpp::display_width(U'\e'));

  EXPECT_EQ(ascpp::display_width("你tnd真是个人才🤡"), 17);
  EXPECT_EQ(ascpp::display_width(L"你tnd真是个人才🤡"), 17);
  EXPECT_EQ(ascpp::display_width(u"你tnd真是个人才🤡"), 17);
  EXPECT_EQ(ascpp::display_width(U"你tnd真是个人才🤡"), 17);
}

TEST(TestMisc, ToInt) {
  EXPECT_EQ(ascpp::to_number<int>("0b10").value(), 2);
  EXPECT_EQ(ascpp::to_number<int>("0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<int>("+0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<int>("-0B10").value(), -2);
  EXPECT_EQ(ascpp::to_number<int>("010").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>("+010").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>("-010").value(), -8);
  EXPECT_EQ(ascpp::to_number<int>("0o10").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>("0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>("+0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>("-0O10").value(), -8);
  EXPECT_EQ(ascpp::to_number<int>("0x10").value(), 16);
  EXPECT_EQ(ascpp::to_number<int>("0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<int>("+0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<int>("-0X10").value(), -16);
  EXPECT_EQ(ascpp::to_number<int>("10").value(), 10);
  EXPECT_EQ(ascpp::to_number<int>("+10").value(), 10);
  EXPECT_EQ(ascpp::to_number<int>("-10").value(), -10);
  EXPECT_EQ(ascpp::to_number<int>("0").value(), 0);
  EXPECT_EQ(ascpp::to_number<int>("+0").value(), 0);
  EXPECT_EQ(ascpp::to_number<int>("-0").value(), 0);
  EXPECT_EQ(ascpp::to_number<int>("+2147483647").value(), std::numeric_limits<int>::max());
  EXPECT_EQ(ascpp::to_number<int>("-2147483648").value(), std::numeric_limits<int>::min());
  EXPECT_EQ(ascpp::to_number<int>("+2147483648").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<int>("-2147483649").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<int>("").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<int>("10x").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<int>("x10").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<int>("1x0").error(), ascpp::error::INVALID_ARGUMENT);

  EXPECT_EQ(ascpp::to_number<int>(L"0b10").value(), 2);
  EXPECT_EQ(ascpp::to_number<int>(L"0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<int>(L"+0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<int>(L"-0B10").value(), -2);
  EXPECT_EQ(ascpp::to_number<int>(L"010").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>(L"+010").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>(L"-010").value(), -8);
  EXPECT_EQ(ascpp::to_number<int>(L"0o10").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>(L"0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>(L"+0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<int>(L"-0O10").value(), -8);
  EXPECT_EQ(ascpp::to_number<int>(L"0x10").value(), 16);
  EXPECT_EQ(ascpp::to_number<int>(L"0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<int>(L"+0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<int>(L"-0X10").value(), -16);
  EXPECT_EQ(ascpp::to_number<int>(L"10").value(), 10);
  EXPECT_EQ(ascpp::to_number<int>(L"+10").value(), 10);
  EXPECT_EQ(ascpp::to_number<int>(L"-10").value(), -10);
  EXPECT_EQ(ascpp::to_number<int>(L"0").value(), 0);
  EXPECT_EQ(ascpp::to_number<int>(L"+0").value(), 0);
  EXPECT_EQ(ascpp::to_number<int>(L"-0").value(), 0);
  EXPECT_EQ(ascpp::to_number<int>(L"+2147483647").value(), std::numeric_limits<int>::max());
  EXPECT_EQ(ascpp::to_number<int>(L"-2147483648").value(), std::numeric_limits<int>::min());
  EXPECT_EQ(ascpp::to_number<int>(L"+2147483648").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<int>(L"-2147483649").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<int>(L"").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<int>(L"10x").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<int>(L"x10").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<int>(L"1x0").error(), ascpp::error::INVALID_ARGUMENT);
}

TEST(TestMisc, ToUll) {
  EXPECT_EQ(ascpp::to_number<size_t>("0b10").value(), 2);
  EXPECT_EQ(ascpp::to_number<size_t>("0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<size_t>("+0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<size_t>("-0B10").value(), -2);
  EXPECT_EQ(ascpp::to_number<size_t>("010").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>("+010").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>("-010").value(), -8);
  EXPECT_EQ(ascpp::to_number<size_t>("0o10").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>("0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>("+0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>("-0O10").value(), -8);
  EXPECT_EQ(ascpp::to_number<size_t>("0x10").value(), 16);
  EXPECT_EQ(ascpp::to_number<size_t>("0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<size_t>("+0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<size_t>("-0X10").value(), -16);
  EXPECT_EQ(ascpp::to_number<size_t>("10").value(), 10);
  EXPECT_EQ(ascpp::to_number<size_t>("+10").value(), 10);
  EXPECT_EQ(ascpp::to_number<size_t>("-10").value(), -10);
  EXPECT_EQ(ascpp::to_number<size_t>("0").value(), 0);
  EXPECT_EQ(ascpp::to_number<size_t>("+0").value(), 0);
  EXPECT_EQ(ascpp::to_number<size_t>("-0").value(), 0);
  EXPECT_EQ(ascpp::to_number<size_t>("+18446744073709551615").value(),
            std::numeric_limits<size_t>::max());
  EXPECT_EQ(ascpp::to_number<size_t>("-1").value(), std::numeric_limits<size_t>::max());
  EXPECT_EQ(ascpp::to_number<size_t>("-18446744073709551615").value(), 1);
  EXPECT_EQ(ascpp::to_number<size_t>("+18446744073709551616").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<size_t>("-18446744073709551616").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<size_t>("").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<size_t>("10x").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<size_t>("x10").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<size_t>("1x0").error(), ascpp::error::INVALID_ARGUMENT);

  EXPECT_EQ(ascpp::to_number<size_t>(L"0b10").value(), 2);
  EXPECT_EQ(ascpp::to_number<size_t>(L"0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+0B10").value(), 2);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-0B10").value(), -2);
  EXPECT_EQ(ascpp::to_number<size_t>(L"010").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+010").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-010").value(), -8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"0o10").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+0O10").value(), 8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-0O10").value(), -8);
  EXPECT_EQ(ascpp::to_number<size_t>(L"0x10").value(), 16);
  EXPECT_EQ(ascpp::to_number<size_t>(L"0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+0X10").value(), 16);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-0X10").value(), -16);
  EXPECT_EQ(ascpp::to_number<size_t>(L"10").value(), 10);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+10").value(), 10);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-10").value(), -10);
  EXPECT_EQ(ascpp::to_number<size_t>(L"0").value(), 0);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+0").value(), 0);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-0").value(), 0);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+18446744073709551615").value(),
            std::numeric_limits<size_t>::max());
  EXPECT_EQ(ascpp::to_number<size_t>(L"-1").value(), std::numeric_limits<size_t>::max());
  EXPECT_EQ(ascpp::to_number<size_t>(L"-18446744073709551615").value(), 1);
  EXPECT_EQ(ascpp::to_number<size_t>(L"+18446744073709551616").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<size_t>(L"-18446744073709551616").error(), ascpp::error::OUT_OF_RANGE);
  EXPECT_EQ(ascpp::to_number<size_t>(L"").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<size_t>(L"10x").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<size_t>(L"x10").error(), ascpp::error::INVALID_ARGUMENT);
  EXPECT_EQ(ascpp::to_number<size_t>(L"1x0").error(), ascpp::error::INVALID_ARGUMENT);
}

TEST(TestMisc, ToFloat) {
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.5").value(), 0.5);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(".5").value(), .5);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.").value(), 0.);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0e1").value(), 0e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.5e1").value(), 0.5e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(".5e1").value(), .5e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.e1").value(), 0.e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0e+1").value(), 0e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.5e+1").value(), 0.5e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(".5e+1").value(), .5e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.e+1").value(), 0.e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0e-1").value(), 0e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.5e-1").value(), 0.5e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(".5e-1").value(), .5e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("0.e-1").value(), 0.e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("3.402823466e+38").value(),
                  std::numeric_limits<float>::max());
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("-3.402823466e+38").value(),
                  std::numeric_limits<float>::lowest());
  EXPECT_FLOAT_EQ(ascpp::to_number<float>("1.175494351e-38").value(),
                  std::numeric_limits<float>::min());
  EXPECT_EQ(ascpp::to_number<float>("inf").value(), std::numeric_limits<float>::infinity());
  EXPECT_EQ(ascpp::to_number<float>("-inf").value(), -std::numeric_limits<float>::infinity());
  EXPECT_TRUE(std::isnan(ascpp::to_number<float>("nan").value()));

  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.5").value(), 0.5);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L".5").value(), .5);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.").value(), 0.);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0e1").value(), 0e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.5e1").value(), 0.5e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L".5e1").value(), .5e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.e1").value(), 0.e1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0e+1").value(), 0e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.5e+1").value(), 0.5e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L".5e+1").value(), .5e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.e+1").value(), 0.e+1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0e-1").value(), 0e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.5e-1").value(), 0.5e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L".5e-1").value(), .5e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"0.e-1").value(), 0.e-1);
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"3.402823466e+38").value(),
                  std::numeric_limits<float>::max());
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"-3.402823466e+38").value(),
                  std::numeric_limits<float>::lowest());
  EXPECT_FLOAT_EQ(ascpp::to_number<float>(L"1.175494351e-38").value(),
                  std::numeric_limits<float>::min());
  EXPECT_EQ(ascpp::to_number<float>(L"inf").value(), std::numeric_limits<float>::infinity());
  EXPECT_EQ(ascpp::to_number<float>(L"-inf").value(), -std::numeric_limits<float>::infinity());
  EXPECT_TRUE(std::isnan(ascpp::to_number<float>(L"nan").value()));
}

// NOLINTEND(modernize-use-trailing-return-type)
