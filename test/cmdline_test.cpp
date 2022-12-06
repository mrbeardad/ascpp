#include "utils/cmdline.hpp"

#include <any>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "gtest/gtest.h"

#include "app.hpp"

TEST(TestCmdline, MapTypeToEnum) {
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<bool>(), ascpp::Option::S_BOOL);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<int>(), ascpp::Option::S_INT);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<size_t>(), ascpp::Option::S_SIZE);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<float>(), ascpp::Option::S_FLOAT);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<double>(), ascpp::Option::S_DOUBLE);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::string>(), ascpp::Option::S_STRING);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::vector<bool>>(), ascpp::Option::M_BOOL);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::vector<int>>(), ascpp::Option::M_INT);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::vector<size_t>>(), ascpp::Option::M_SIZE);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::vector<float>>(), ascpp::Option::M_FLOAT);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::vector<double>>(), ascpp::Option::M_DOUBLE);
  EXPECT_EQ(ascpp::Option::MapTypeToEnum<std::vector<std::string>>(), ascpp::Option::M_STRING);
}

TEST(TestCmdline, MapTypeEnumToStr) {
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::S_BOOL), "bool");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::S_INT), "int");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::S_SIZE), "size_t");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::S_FLOAT), "float");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::S_DOUBLE), "double");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::S_STRING), "string");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::M_BOOL), "list of bool");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::M_INT), "list of int");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::M_SIZE), "list of size_t");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::M_FLOAT), "list of float");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::M_DOUBLE), "list of double");
  EXPECT_EQ(ascpp::Option::MapTypeEnumToStr(ascpp::Option::M_STRING), "list of string");
}

TEST(TestCmdline, AddSingleOptions) {
  auto cmd = ascpp::Cmdline(&app_info);

  cmd.AddOption<bool>('b', "bool", "bool desc");
  auto& b = cmd.GetOption('b');
  auto& bol = cmd.GetOption("bool");
  EXPECT_EQ(&b, &bol);
  EXPECT_EQ(bol.opt_type, ascpp::Option::S_BOOL);
  EXPECT_EQ(bol.short_opt, "b");
  EXPECT_EQ(bol.long_opt, "bool");
  EXPECT_EQ(bol.opt_desc, "bool desc");
  EXPECT_EQ(bol.limits.has_value(), false);
  EXPECT_EQ(std::any_cast<bool>(bol.default_value), false);
  EXPECT_EQ(std::any_cast<bool>(bol.implicit_value), true);

  cmd.AddOption<int>("int", "int desc");
  auto& intt = cmd.GetOption("int");
  EXPECT_EQ(intt.opt_type, ascpp::Option::S_INT);
  EXPECT_EQ(intt.short_opt, "");
  EXPECT_EQ(intt.long_opt, "int");
  EXPECT_EQ(intt.opt_desc, "int desc");
  EXPECT_EQ(intt.limits.has_value(), false);
  EXPECT_EQ(intt.default_value.has_value(), false);
  EXPECT_EQ(intt.implicit_value.has_value(), false);

  auto limit_set_adder = cmd.AddOption<size_t>("size_t", "size_t desc").WithLimits({0, 1, 2});
  auto& size = cmd.GetOption("size_t");
  EXPECT_EQ(size.opt_type, ascpp::Option::S_SIZE);
  EXPECT_EQ(size.short_opt, "");
  EXPECT_EQ(size.long_opt, "size_t");
  EXPECT_EQ(size.opt_desc, "size_t desc");
  auto size_set = std::unordered_set<size_t>{0, 1, 2};
  EXPECT_EQ(std::any_cast<const std::unordered_set<size_t>&>(size.limits), size_set);
  EXPECT_EQ(size.default_value.has_value(), false);
  EXPECT_EQ(size.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_set_adder.WithDefault(3));
  EXPECT_ANY_THROW(limit_set_adder.WithImplicit(-1));
  limit_set_adder.WithDefault(0);
  EXPECT_EQ(std::any_cast<size_t>(size.default_value), 0);
  limit_set_adder.WithImplicit(1);
  EXPECT_EQ(std::any_cast<size_t>(size.implicit_value), 1);

  auto limit_pred_adder
      = cmd.AddOption<float>("float", "float desc").WithLimits([](auto v) { return v > 0; });
  auto& flt = cmd.GetOption("float");
  EXPECT_EQ(flt.opt_type, ascpp::Option::S_FLOAT);
  EXPECT_EQ(flt.short_opt, "");
  EXPECT_EQ(flt.long_opt, "float");
  EXPECT_EQ(flt.opt_desc, "float desc");
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(1), true);
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(0), false);
  EXPECT_EQ(flt.default_value.has_value(), false);
  EXPECT_EQ(flt.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_pred_adder.WithDefault(-1));
  EXPECT_ANY_THROW(limit_pred_adder.WithImplicit(0));
  limit_pred_adder.WithDefault(1);
  EXPECT_FLOAT_EQ(std::any_cast<float>(flt.default_value), 1);
  limit_pred_adder.WithImplicit(2);
  EXPECT_FLOAT_EQ(std::any_cast<float>(flt.implicit_value), 2);

  cmd.AddOption<double>("double", "double desc").WithDefault(1.5);
  auto& dbl = cmd.GetOption("double");
  EXPECT_EQ(dbl.opt_type, ascpp::Option::S_DOUBLE);
  EXPECT_EQ(dbl.short_opt, "");
  EXPECT_EQ(dbl.long_opt, "double");
  EXPECT_EQ(dbl.opt_desc, "double desc");
  EXPECT_EQ(dbl.limits.has_value(), false);
  EXPECT_DOUBLE_EQ(std::any_cast<double>(dbl.default_value), 1.5);
  EXPECT_EQ(dbl.implicit_value.has_value(), false);

  cmd.AddOption<std::string>("string", "string desc").WithImplicit("str");
  auto& str = cmd.GetOption("string");
  EXPECT_EQ(str.opt_type, ascpp::Option::S_STRING);
  EXPECT_EQ(str.short_opt, "");
  EXPECT_EQ(str.long_opt, "string");
  EXPECT_EQ(str.opt_desc, "string desc");
  EXPECT_EQ(str.limits.has_value(), false);
  EXPECT_EQ(str.default_value.has_value(), false);
  EXPECT_EQ(std::any_cast<std::string>(str.implicit_value), "str");

  std::clog << cmd.HelpString() << std::endl;
}

TEST(TestCmdline, AddMultiOptions) {
  auto cmd = ascpp::Cmdline(&app_info);

  cmd.AddOption<std::vector<bool>>('b', "bool", "bool desc");
  auto& b = cmd.GetOption('b');
  auto& bol = cmd.GetOption("bool");
  EXPECT_EQ(&b, &bol);
  EXPECT_EQ(bol.opt_type, ascpp::Option::M_BOOL);
  EXPECT_EQ(bol.short_opt, "b");
  EXPECT_EQ(bol.long_opt, "bool");
  EXPECT_EQ(bol.opt_desc, "bool desc");
  EXPECT_EQ(bol.limits.has_value(), false);
  EXPECT_EQ(bol.default_value.has_value(), false);
  EXPECT_EQ(bol.implicit_value.has_value(), false);

  cmd.AddOption<std::vector<int>>("int", "int desc");
  auto& intt = cmd.GetOption("int");
  EXPECT_EQ(intt.opt_type, ascpp::Option::M_INT);
  EXPECT_EQ(intt.short_opt, "");
  EXPECT_EQ(intt.long_opt, "int");
  EXPECT_EQ(intt.opt_desc, "int desc");
  EXPECT_EQ(intt.limits.has_value(), false);
  EXPECT_EQ(intt.default_value.has_value(), false);
  EXPECT_EQ(intt.implicit_value.has_value(), false);

  auto limit_set_adder
      = cmd.AddOption<std::vector<size_t>>("size_t", "size_t desc").WithLimits({0, 1, 2});
  auto& size = cmd.GetOption("size_t");
  EXPECT_EQ(size.opt_type, ascpp::Option::M_SIZE);
  EXPECT_EQ(size.short_opt, "");
  EXPECT_EQ(size.long_opt, "size_t");
  EXPECT_EQ(size.opt_desc, "size_t desc");
  auto size_set = std::unordered_set<size_t>{0, 1, 2};
  EXPECT_EQ(std::any_cast<const std::unordered_set<size_t>&>(size.limits), size_set);
  EXPECT_EQ(size.default_value.has_value(), false);
  EXPECT_EQ(size.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_set_adder.WithDefault({3}));
  EXPECT_ANY_THROW(limit_set_adder.WithImplicit({2, 3}));
  auto size_vec = std::vector<size_t>{1};
  limit_set_adder.WithDefault({1});
  EXPECT_EQ(std::any_cast<const std::vector<size_t>&>(size.default_value), size_vec);
  size_vec = {0, 1, 2};
  limit_set_adder.WithImplicit({0, 1, 2});
  EXPECT_EQ(std::any_cast<const std::vector<size_t>&>(size.implicit_value), size_vec);

  auto limit_pred_adder
      = cmd.AddOption<std::vector<float>>("float", "float desc").WithLimits([](auto v) {
          return v > 0;
        });
  auto& flt = cmd.GetOption("float");
  EXPECT_EQ(flt.opt_type, ascpp::Option::M_FLOAT);
  EXPECT_EQ(flt.short_opt, "");
  EXPECT_EQ(flt.long_opt, "float");
  EXPECT_EQ(flt.opt_desc, "float desc");
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(1), true);
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(0), false);
  EXPECT_EQ(flt.default_value.has_value(), false);
  EXPECT_EQ(flt.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_pred_adder.WithDefault({0, 1}));
  EXPECT_ANY_THROW(limit_pred_adder.WithImplicit({-1}));
  limit_pred_adder.WithDefault({1}).WithImplicit({2, 3});
  ;
  auto flt_vec = std::vector<float>{1};
  EXPECT_EQ(std::any_cast<const std::vector<float>&>(flt.default_value), flt_vec);
  flt_vec = {2, 3};
  EXPECT_EQ(std::any_cast<const std::vector<float>&>(flt.implicit_value), flt_vec);

  cmd.AddOption<std::vector<double>>("double", "double desc").WithDefault({1.5});
  auto& dbl = cmd.GetOption("double");
  EXPECT_EQ(dbl.opt_type, ascpp::Option::M_DOUBLE);
  EXPECT_EQ(dbl.short_opt, "");
  EXPECT_EQ(dbl.long_opt, "double");
  EXPECT_EQ(dbl.opt_desc, "double desc");
  EXPECT_EQ(dbl.limits.has_value(), false);
  auto dbl_vec = std::vector<double>{1.5};
  EXPECT_EQ(std::any_cast<const std::vector<double>&>(dbl.default_value), dbl_vec);
  EXPECT_EQ(dbl.implicit_value.has_value(), false);

  cmd.AddOption<std::vector<std::string>>("string", "string desc").WithImplicit({"str"});
  auto& str = cmd.GetOption("string");
  EXPECT_EQ(str.opt_type, ascpp::Option::M_STRING);
  EXPECT_EQ(str.short_opt, "");
  EXPECT_EQ(str.long_opt, "string");
  EXPECT_EQ(str.opt_desc, "string desc");
  EXPECT_EQ(str.limits.has_value(), false);
  EXPECT_EQ(str.default_value.has_value(), false);
  auto str_vec = std::vector<std::string>{"str"};
  EXPECT_EQ(std::any_cast<const std::vector<std::string>&>(str.implicit_value), str_vec);

  std::clog << cmd.HelpString() << std::endl;
}

TEST(TestCmdline, AddBadOptions) {
  auto cmd = ascpp::Cmdline{&app_info};

  EXPECT_ANY_THROW(cmd.AddOption<bool>('\0', "zero", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>('\t', "tab", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>('\n', "newline", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>(' ', "space", ""));
  cmd.AddOption<bool>('=', "punct", "");
  EXPECT_EQ(cmd.GetOption('=').short_opt, "=");
  EXPECT_EQ(cmd.GetOption('=').long_opt, "punct");

  EXPECT_ANY_THROW(cmd.AddOption<bool>("1", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("with space", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("with=", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("=with", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("选项", ""));
}

TEST(TestCmdline, BoolBasic) {
  auto cmd = ascpp::Cmdline{&app_info};
  auto args = std::vector<const char*>();

  cmd.AddOption<bool>('o', "opt", "bool option");
  args = {"ascpp"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

  args = {"ascpp", "-o"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

  // bool short option requires no value
  args = {"ascpp", "-ofalse"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o", "false"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"false"});

  args = {"ascpp", "--opt"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

  args = {"ascpp", "--opt", "false"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"false"});

  args = {"ascpp", "--opt=true"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

  args = {"ascpp", "--opt=false"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

  args = {"ascpp", "--opt=yes"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

  args = {"ascpp", "--opt=no"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

  args = {"ascpp", "--opt=on"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

  args = {"ascpp", "--opt=off"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

  args = {"ascpp", "--opt=1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

  args = {"ascpp", "--opt=0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

  args = {"ascpp", "--opt=other"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=2"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=-1"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
}

TEST(TestCmdline, IntBasic) {
  auto cmd = ascpp::Cmdline{&app_info};
  auto args = std::vector<const char*>();

  cmd.AddOption<int>('o', "opt", "int option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 0);

  args = {"ascpp", "-o2147483647"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MAX);

  args = {"ascpp", "-o2147483648"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o-2147483648"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MIN);

  args = {"ascpp", "-o-2147483649"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o", "0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 0);

  args = {"ascpp", "-o", "2147483647"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MAX);

  args = {"ascpp", "-o", "2147483648"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o", "-2147483648"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MIN);

  args = {"ascpp", "-o", "-2147483649"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 1);

  args = {"ascpp", "--opt", "2147483647"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MAX);

  args = {"ascpp", "--opt", "2147483648"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "-2147483648"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MIN);

  args = {"ascpp", "--opt", "-2147483649"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 1);

  args = {"ascpp", "--opt=2147483647"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MAX);

  args = {"ascpp", "--opt=2147483648"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=-2147483648"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), INT32_MIN);

  args = {"ascpp", "--opt=-2147483649"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
}

TEST(TestCmdline, SizeBasic) {
  auto cmd = ascpp::Cmdline{&app_info};
  auto args = std::vector<const char*>();

  cmd.AddOption<size_t>('o', "opt", "size_t option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 0);

  args = {"ascpp", "-o18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "-o18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  // unsigned integer wraparound rules
  args = {"ascpp", "-o-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "-o-18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "-o-18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o", "0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 0);

  args = {"ascpp", "-o", "18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "-o", "18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o", "-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "-o", "-18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "-o", "-18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "--opt", "18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "--opt", "18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "--opt", "-18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "--opt", "-18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "--opt=18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "--opt=18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), UINT64_MAX);

  args = {"ascpp", "--opt=-18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "--opt=-18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
}

/*
TEST(TestCmdline, BasicUsage) {
  auto cmdline = ascpp::Cmdline(&app_info);
  cmdline.AddOption<std::string>('s', "str", "");
  auto argv = std::vector<const char*>();
  auto unmatched = std::vector<std::string>();

  argv = {"ascpp", "-shello"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "hello");

  argv = {"ascpp", "-s", "hello"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "hello");

  argv = {"ascpp", "--str=hello"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "hello");

  argv = {"ascpp", "--str=hello", "world"};
  unmatched = {"world"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline.GetUnmatchedArgs(), unmatched);

  argv = {"ascpp", "--str=hello", "my", "--str=world"};
  unmatched = {"my"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "world");
  EXPECT_EQ(cmdline.GetUnmatchedArgs(), unmatched);

  argv = {"ascpp", "--str=hello", "--", "--str=world"};
  unmatched = {"--str=world"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline.GetUnmatchedArgs(), unmatched);
}

TEST(TestCmdline, AddBoolOption) {
  auto cmdline = ascpp::Cmdline{&app_info};
  cmdline.AddOption<bool>('b', "short_b", "");
  cmdline.AddOption<bool>("bool", "");
  auto cmdline_default = ascpp::Cmdline{&app_info};
  cmdline_default.AddOptionWithDefault<bool>('b', "short_b", "", true);
  cmdline_default.AddOptionWithDefault<bool>("bool", "", true);
  auto cmdline_implicit = ascpp::Cmdline{&app_info};
  cmdline_implicit.AddOptionWithImplicit<bool>('b', "short_b", "", false);
  cmdline_implicit.AddOptionWithImplicit<bool>("bool", "", false);
  auto cmdline_both = ascpp::Cmdline{&app_info};
  cmdline_both.AddOptionWithBoth<bool>('b', "short_b", "", true, false);
  cmdline_both.AddOptionWithBoth<bool>("bool", "", true, false);
  auto argv = std::vector<const char*>();

  // bool has default default_value: false
  argv = {"ascpp"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("b"), false);
  EXPECT_EQ(cmdline.GetOptionValue<bool>("short_b"), false);
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), false);

  // set default_value to true
  argv = {"ascpp"};
  cmdline_default.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.GetOptionValue<bool>("b"), true);
  EXPECT_EQ(cmdline_default.GetOptionValue<bool>("short_b"), true);
  EXPECT_EQ(cmdline_default.GetOptionValue<bool>("bool"), true);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<bool>("b"), true);
  EXPECT_EQ(cmdline_both.GetOptionValue<bool>("short_b"), true);
  EXPECT_EQ(cmdline_both.GetOptionValue<bool>("bool"), true);

  // bool has default implicit_value: true
  argv = {"ascpp", "-b"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("b"), true);
  EXPECT_EQ(cmdline.GetOptionValue<bool>("short_b"), true);
  argv = {"ascpp", "--bool"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), true);

  // set implicit_value to false
  argv = {"ascpp", "-b"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<bool>("b"), false);
  EXPECT_EQ(cmdline_implicit.GetOptionValue<bool>("short_b"), false);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<bool>("b"), false);
  EXPECT_EQ(cmdline_both.GetOptionValue<bool>("short_b"), false);
  argv = {"ascpp", "--bool"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<bool>("bool"), false);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<bool>("bool"), false);

  // unable to set the value of short options that has implicit_value
  argv = {"ascpp", "-btrue"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "-bfalse"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "-b", "true"};
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), true);
  argv = {"ascpp", "-b", "false"};
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), true);

  // set the value of the long option
  argv = {"ascpp", "--bool=true"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), true);
  argv = {"ascpp", "--bool=1"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), true);
  argv = {"ascpp", "--bool=false"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), false);
  argv = {"ascpp", "--bool=0"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<bool>("bool"), false);
}

TEST(TestCmdline, AddIntOption) {
  auto cmdline = ascpp::Cmdline(&app_info);
  cmdline.AddOption<int>('i', "short_i", "");
  cmdline.AddOption<int>("int", "");
  auto cmdline_default = ascpp::Cmdline(&app_info);
  cmdline_default.AddOptionWithDefault<int>('i', "short_i", "", 11);
  cmdline_default.AddOptionWithDefault<int>("int", "", 11);
  auto cmdline_implicit = ascpp::Cmdline(&app_info);
  cmdline_implicit.AddOptionWithImplicit<int>('i', "short_i", "", 11);
  cmdline_implicit.AddOptionWithImplicit<int>("int", "", 11);
  auto cmdline_both = ascpp::Cmdline(&app_info);
  cmdline_both.AddOptionWithBoth<int>('i', "short_i", "", 11, 11);
  cmdline_both.AddOptionWithBoth<int>("int", "", 11, 11);
  auto argv = std::vector<const char*>();

  // without default_value
  argv = {"ascpp"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.GetOptionValue<int>("i"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<int>("short_i"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<int>("int"));

  // with default_value 11
  argv = {"ascpp"};
  cmdline_default.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.GetOptionValue<int>("i"), 11);
  EXPECT_EQ(cmdline_default.GetOptionValue<int>("short_i"), 11);
  EXPECT_EQ(cmdline_default.GetOptionValue<int>("int"), 11);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<int>("i"), 11);
  EXPECT_EQ(cmdline_both.GetOptionValue<int>("short_i"), 11);
  EXPECT_EQ(cmdline_both.GetOptionValue<int>("int"), 11);

  // without implicit_value
  argv = {"ascpp", "-i"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "--int"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));

  // with implicit_value 11
  argv = {"ascpp", "-i"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<int>("i"), 11);
  EXPECT_EQ(cmdline_implicit.GetOptionValue<int>("short_i"), 11);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<int>("i"), 11);
  EXPECT_EQ(cmdline_both.GetOptionValue<int>("short_i"), 11);
  argv = {"ascpp", "--int"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<int>("int"), 11);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<int>("int"), 11);

  // set the value of short option without implicit value
  argv = {"ascpp", "-i22"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<int>("i"), 22);
  EXPECT_EQ(cmdline.GetOptionValue<int>("short_i"), 22);
  argv = {"ascpp", "-i", "22"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<int>("i"), 22);
  EXPECT_EQ(cmdline.GetOptionValue<int>("short_i"), 22);

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-i22"};
  EXPECT_ANY_THROW(cmdline_implicit.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "-i", "22"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<int>("i"), 11);
  EXPECT_EQ(cmdline_implicit.GetOptionValue<int>("short_i"), 11);

  // set the argument of the long option
  argv = {"ascpp", "--int=22"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<int>("int"), 22);

  argv = {"ascpp", "--int=nan"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
}

TEST(TestCmdline, AddFloatOption) {
  auto cmdline = ascpp::Cmdline(&app_info);
  cmdline.AddOption<float>('f', "short_f", "");
  cmdline.AddOption<float>("float", "");
  auto cmdline_default = ascpp::Cmdline(&app_info);
  cmdline_default.AddOptionWithDefault<float>('f', "short_f", "", 1.1);
  cmdline_default.AddOptionWithDefault<float>("float", "", 1.1);
  auto cmdline_implicit = ascpp::Cmdline(&app_info);
  cmdline_implicit.AddOptionWithImplicit<float>('f', "short_f", "", 1.1);
  cmdline_implicit.AddOptionWithImplicit<float>("float", "", 1.1);
  auto cmdline_both = ascpp::Cmdline(&app_info);
  cmdline_both.AddOptionWithBoth<float>('f', "short_f", "", 1.1, 1.1);
  cmdline_both.AddOptionWithBoth<float>("float", "", 1.1, 1.1);
  auto argv = std::vector<const char*>();

  // without default_value
  argv = {"ascpp"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.GetOptionValue<float>("f"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<float>("short_f"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<float>("float"));

  // with default_value 1.1
  argv = {"ascpp"};
  cmdline_default.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_default.GetOptionValue<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_default.GetOptionValue<float>("short_f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_default.GetOptionValue<float>("float"), 1.1);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_both.GetOptionValue<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_both.GetOptionValue<float>("short_f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_both.GetOptionValue<float>("float"), 1.1);

  // without implicit_value
  argv = {"ascpp", "-f"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "--float"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));

  // with implicit_value 1.1
  argv = {"ascpp", "-f"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_implicit.GetOptionValue<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_implicit.GetOptionValue<float>("short_f"), 1.1);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_both.GetOptionValue<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_both.GetOptionValue<float>("short_f"), 1.1);
  argv = {"ascpp", "--float"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_implicit.GetOptionValue<float>("float"), 1.1);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_both.GetOptionValue<float>("float"), 1.1);

  // set the value of short option without implicit value
  argv = {"ascpp", "-f2.2"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline.GetOptionValue<float>("f"), 2.2);
  EXPECT_FLOAT_EQ(cmdline.GetOptionValue<float>("short_f"), 2.2);
  argv = {"ascpp", "-f", "2.2"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline.GetOptionValue<float>("f"), 2.2);
  EXPECT_FLOAT_EQ(cmdline.GetOptionValue<float>("short_f"), 2.2);

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-f2.2"};
  EXPECT_ANY_THROW(cmdline_implicit.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "-f", "2.2"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_implicit.GetOptionValue<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_implicit.GetOptionValue<float>("short_f"), 1.1);

  // set the argument of the long option
  argv = {"ascpp", "--float=2.2"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline.GetOptionValue<float>("float"), 2.2);
}

TEST(TestCmdline, AddStringOption) {
  auto cmdline = ascpp::Cmdline(&app_info);
  cmdline.AddOption<std::string>('s', "short_s", "");
  cmdline.AddOption<std::string>("string", "");
  auto cmdline_default = ascpp::Cmdline(&app_info);
  cmdline_default.AddOptionWithDefault<std::string>('s', "short_s", "", "hello");
  cmdline_default.AddOptionWithDefault<std::string>("string", "", "hello");
  auto cmdline_implicit = ascpp::Cmdline(&app_info);
  cmdline_implicit.AddOptionWithImplicit<std::string>('s', "short_s", "", "hello");
  cmdline_implicit.AddOptionWithImplicit<std::string>("string", "", "hello");
  auto cmdline_both = ascpp::Cmdline(&app_info);
  cmdline_both.AddOptionWithBoth<std::string>('s', "short_s", "", "hello", "hello");
  cmdline_both.AddOptionWithBoth<std::string>("string", "", "hello", "hello");
  auto argv = std::vector<const char*>();

  // without default_value
  argv = {"ascpp"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.GetOptionValue<std::string>("s"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<std::string>("short_s"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<std::string>("string"));

  // with default_value "hello"
  argv = {"ascpp"};
  cmdline_default.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_default.GetOptionValue<std::string>("short_s"), "hello");
  EXPECT_EQ(cmdline_default.GetOptionValue<std::string>("string"), "hello");
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_both.GetOptionValue<std::string>("short_s"), "hello");
  EXPECT_EQ(cmdline_both.GetOptionValue<std::string>("string"), "hello");

  // without implicit_value
  argv = {"ascpp", "-s"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "--string"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));

  // with implicit_value "hello"
  argv = {"ascpp", "-s"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::string>("short_s"), "hello");
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_both.GetOptionValue<std::string>("short_s"), "hello");
  argv = {"ascpp", "--string"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::string>("string"), "hello");
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<std::string>("string"), "hello");

  // set the value of short option without implicit value
  argv = {"ascpp", "-sworld"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "world");
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("short_s"), "world");
  argv = {"ascpp", "-s", "world"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("s"), "world");
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("short_s"), "world");

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-sworld"};
  EXPECT_ANY_THROW(cmdline_implicit.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "-s", "world"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::string>("short_s"), "hello");

  // set the argument of the long option
  argv = {"ascpp", "--string=world"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::string>("string"), "world");
}

TEST(TestCmdline, AddVectorOption) {
  auto cmdline = ascpp::Cmdline(&app_info);
  cmdline.AddOption<std::vector<std::string>>('V', "short_v", "");
  cmdline.AddOption<std::vector<std::string>>("vector", "");
  auto cmdline_default = ascpp::Cmdline(&app_info);
  cmdline_default.AddOptionWithDefault<std::vector<std::string>>('V', "short_v", "",
                                                                 {"a", "b", "c"});
  cmdline_default.AddOptionWithDefault<std::vector<std::string>>("vector", "", {"a", "b", "c"});
  auto cmdline_implicit = ascpp::Cmdline(&app_info);
  cmdline_implicit.AddOptionWithImplicit<std::vector<std::string>>('V', "short_v", "",
                                                                   {"a", "b", "c"});
  cmdline_implicit.AddOptionWithImplicit<std::vector<std::string>>("vector", "", {"a", "b", "c"});
  auto cmdline_both = ascpp::Cmdline(&app_info);
  cmdline_both.AddOptionWithBoth<std::vector<std::string>>('V', "short_v", "", {"a", "b", "c"},
                                                           {"a", "b", "c"});
  cmdline_both.AddOptionWithBoth<std::vector<std::string>>("vector", "", {"a", "b", "c"},
                                                           {"a", "b", "c"});
  auto argv = std::vector<const char*>();
  auto values = std::vector<std::string>{"a", "b", "c"};

  // without default_value
  argv = {"ascpp"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.GetOptionValue<std::vector<std::string>>("V"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<std::vector<std::string>>("short_v"));
  EXPECT_ANY_THROW(cmdline.GetOptionValue<std::vector<std::string>>("vector"));

  // with default_value {"a","b","c"}
  argv = {"ascpp"};
  cmdline_default.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_default.GetOptionValue<std::vector<std::string>>("short_v"), values);
  EXPECT_EQ(cmdline_default.GetOptionValue<std::vector<std::string>>("vector"), values);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_both.GetOptionValue<std::vector<std::string>>("short_v"), values);
  EXPECT_EQ(cmdline_both.GetOptionValue<std::vector<std::string>>("vector"), values);

  // without implicit_value
  argv = {"ascpp", "-V"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "--vector"};
  EXPECT_ANY_THROW(cmdline.ParseArgs(argv.size(), argv.data()));

  // with implicit_value {"a","b","c"}
  argv = {"ascpp", "-V"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::vector<std::string>>("short_v"), values);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_both.GetOptionValue<std::vector<std::string>>("short_v"), values);
  argv = {"ascpp", "--vector"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::vector<std::string>>("vector"), values);
  cmdline_both.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.GetOptionValue<std::vector<std::string>>("vector"), values);

  // set the value of short option without implicit value
  argv = {"ascpp", "-Va,b,c"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("short_v"), values);
  argv = {"ascpp", "-V", "a,b,c"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("short_v"), values);

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-Vx,y,z"};
  EXPECT_ANY_THROW(cmdline_implicit.ParseArgs(argv.size(), argv.data()));
  argv = {"ascpp", "-V", "x,y,z"};
  cmdline_implicit.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_implicit.GetOptionValue<std::vector<std::string>>("short_v"), values);

  // set the argument of the long option
  argv = {"ascpp", "--vector=a,b,c"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("vector"), values);
  argv = {"ascpp", "--vector=a,b,c,"};
  cmdline.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("vector"), values);
  argv = {"ascpp", "--vector=a,b,c,,"};
  cmdline.ParseArgs(argv.size(), argv.data());
  values.emplace_back("");
  EXPECT_EQ(cmdline.GetOptionValue<std::vector<std::string>>("vector"), values);
}

TEST(TestCmdline, AddPositionalOption) {
  auto option = ascpp::Cmdline(&app_info);
  auto argv = std::vector<const char*>();
  auto value = std::vector<std::string>();
  option.AddOption<bool>('o', "opt", "");
  option.AddOption<std::string>("a", "", true);
  option.AddOption<std::string>("b", "", true);
  option.AddOption<std::vector<std::string>>("c", "", true);

  argv = {"ascpp", "a", "b", "c"};
  option.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(option.GetOptionValue<std::string>("a"), "a");
  EXPECT_EQ(option.GetOptionValue<std::string>("b"), "b");
  value = {"c"};
  EXPECT_EQ(option.GetOptionValue<std::vector<std::string>>("c"), value);

  argv = {"ascpp", "a", "b", "c", "d"};
  option.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(option.GetOptionValue<std::string>("a"), "a");
  EXPECT_EQ(option.GetOptionValue<std::string>("b"), "b");
  value = {"c", "d"};
  EXPECT_EQ(option.GetOptionValue<std::vector<std::string>>("c"), value);

  // FIXME: should throw when there is no positional arguments for option c
  argv = {"ascpp", "a", "b"};
  // EXPECT_ANY_THROW(option.Parse(argv.size(), argv.data()));
  option.ParseArgs(argv.size(), argv.data());
  EXPECT_EQ(option.GetOptionValue<std::string>("a"), "a");
  EXPECT_EQ(option.GetOptionValue<std::string>("b"), "b");
  value = {};
  EXPECT_ANY_THROW(option.GetOptionValue<std::vector<std::string>>("c"));
}

TEST(TestCmdline, HelpAndVersionOption) {
  auto option = ascpp::Cmdline(&app_info);
  auto argv = std::vector<const char*>();

  // argv = {"ascpp", "--help"};
  // option.Parse(argv.size(), argv.data());

  // argv = {"ascpp", "--version"};
  // option.Parse(argv.size(), argv.data());
}
*/
