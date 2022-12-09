#include "utils/cmdline.hpp"
#include <float.h>
#include <vcruntime.h>

#include <any>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
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
  EXPECT_EQ(std::any_cast<float>(flt.default_value), 1);
  limit_pred_adder.WithImplicit(2);
  EXPECT_EQ(std::any_cast<float>(flt.implicit_value), 2);

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

  cmd.AddOption<bool>('o', "option", "normal option");
  EXPECT_ANY_THROW(cmd.AddOption<bool>("option", "duplicate long option name"));
  EXPECT_ANY_THROW(cmd.AddOption<bool>('o', "longoption", "duplicate short option name"));

  EXPECT_ANY_THROW(cmd.AddOption<bool>('\0', "zero", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>('\n', "newline", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>('\t', "tab", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>(' ', "space", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>('-', "hyphen", ""));
  cmd.AddOption<bool>('=', "equal", "allowed = in short option");
  EXPECT_EQ(cmd.GetOption('=').short_opt, "=");
  EXPECT_EQ(cmd.GetOption('=').long_opt, "equal");

  EXPECT_ANY_THROW(cmd.AddOption<bool>("1", "too short, at least two char"));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("with\nnewline", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("with\ttab", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("with space", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("with=", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("=with", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("wi=th", ""));
  EXPECT_ANY_THROW(cmd.AddOption<bool>("中文选项", ""));
  cmd.AddOption<bool>("-hyphen", "allowed - in long option");
  EXPECT_EQ(cmd.GetOption("-hyphen").short_opt, "");
  EXPECT_EQ(cmd.GetOption("-hyphen").long_opt, "-hyphen");
  cmd.AddOption<bool>("hyphen-", "allowed - in long option");
  EXPECT_EQ(cmd.GetOption("hyphen-").short_opt, "");
  EXPECT_EQ(cmd.GetOption("hyphen-").long_opt, "hyphen-");
  cmd.AddOption<bool>("hy-phen", "allowed - in long option");
  EXPECT_EQ(cmd.GetOption("hy-phen").short_opt, "");
  EXPECT_EQ(cmd.GetOption("hy-phen").long_opt, "hy-phen");
}

TEST(TestCmdline, BasicParse) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<bool>('n', "non", "bool short option requires no value");
  cmd.AddOption<std::string>('o', "optional",
                             "short option with implicit value optional need a value")
      .WithDefault("default")
      .WithImplicit("implicit");
  cmd.AddOption<std::string>('r', "required",
                             "short option without implicit value requires a value")
      .WithDefault("default");
  cmd.AllowNonOptions("nonopts", false);

  args = {"ascpp"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), false);
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "default");
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "default");

  args = {"ascpp", "-VALUE"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-n"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);

  args = {"ascpp", "-nVALUE"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-n", "VALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-o"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "implicit");

  args = {"ascpp", "-oVALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "VALUE");

  args = {"ascpp", "-o", "VALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-r"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-rVALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "VALUE");

  args = {"ascpp", "-r", "-n"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "-n");

  args = {"ascpp", "-r", "--non"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "--non");

  args = {"ascpp", "-noVALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "VALUE");

  args = {"ascpp", "-no", "VALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-nrVALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "VALUE");

  args = {"ascpp", "-nr", "-VALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "-VALUE");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{});

  args = {"ascpp", "-norVALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "rVALUE");
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "default");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{});

  args = {"ascpp", "-nor", "VALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "r");
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "default");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-nroVALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "oVALUE");
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "default");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{});

  args = {"ascpp", "-nro", "VALUE"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "default");
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "o");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "--non"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);

  args = {"ascpp", "--non="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), false);

  args = {"ascpp", "--non=false"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), false);

  args = {"ascpp", "--non", "true"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("n"), true);
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"true"});

  args = {"ascpp", "--optional"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "implicit");

  args = {"ascpp", "--optional="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "");

  args = {"ascpp", "--optional=false"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "false");

  args = {"ascpp", "--optional", "true"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.GetNonOptions(), std::vector<std::string>{"true"});

  args = {"ascpp", "--required"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--required="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "");

  args = {"ascpp", "--required==false"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "=false");

  args = {"ascpp", "--required", "--non"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "--non");

  args = {"ascpp", "--required", "-n"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "-n");

  args = {"ascpp", "0",         "-r", "true", "1",     "-o",      "2",
          "--",    "all after", "--", "are",  "--non", "-option", "arguments"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("r"), "true");
  EXPECT_EQ(cmd.GetValue<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.GetValue<bool>("n"), false);
  auto nonoptions = std::vector<std::string>{"0",   "1",     "2",       "all after", "--",
                                             "are", "--non", "-option", "arguments"};
  EXPECT_EQ(cmd.GetNonOptions(), nonoptions);

  std::clog << cmd.HelpString() << std::endl;
}

TEST(TestCmdline, BoolOption) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<bool>('o', "opt", "bool option");
  cmd.AllowNonOptions("nonopts", false);

  args = {"ascpp"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

  args = {"ascpp", "-o"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), true);

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

  args = {"ascpp", "--opt="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<bool>("opt"), false);

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

TEST(TestCmdline, IntOption) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<int>('o', "opt", "int option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 1);

  args = {"ascpp", "-o", "1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 1);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 1);

  args = {"ascpp", "--opt="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 0);

  args = {"ascpp", "--opt=0b10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 2);

  args = {"ascpp", "--opt=0B10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 2);

  args = {"ascpp", "--opt=0o10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 8);

  args = {"ascpp", "--opt=0O10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 8);

  args = {"ascpp", "--opt=010"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 8);

  args = {"ascpp", "--opt=0x10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 16);

  args = {"ascpp", "--opt=0x10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 16);

  args = {"ascpp", "--opt=0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 0);

  args = {"ascpp", "--opt=+0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 0);

  args = {"ascpp", "--opt=-0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), 0);

  args = {"ascpp", "--opt=+2147483647"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), std::numeric_limits<int>::max());

  args = {"ascpp", "--opt=+2147483648"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=-2147483648"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<int>("opt"), std::numeric_limits<int>::min());

  args = {"ascpp", "--opt=-2147483649"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=not-number"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=12not-number"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=not-number12"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
}

TEST(TestCmdline, SizeOption) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<size_t>('o', "opt", "size_t option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "-o", "1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "--opt="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 0);

  args = {"ascpp", "--opt=0b10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 2);

  args = {"ascpp", "--opt=0B10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 2);

  args = {"ascpp", "--opt=0o10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 8);

  args = {"ascpp", "--opt=0O10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 8);

  args = {"ascpp", "--opt=010"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 8);

  args = {"ascpp", "--opt=0x10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 16);

  args = {"ascpp", "--opt=0x10"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 16);

  args = {"ascpp", "--opt=0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 0);

  args = {"ascpp", "--opt=+0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 0);

  args = {"ascpp", "--opt=-0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 0);

  args = {"ascpp", "-o18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), std::numeric_limits<size_t>::max());

  args = {"ascpp", "-o18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  // unsigned integer wraparound rules
  args = {"ascpp", "-o-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), std::numeric_limits<size_t>::max());

  args = {"ascpp", "-o-18446744073709551615"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<size_t>("opt"), 1);

  args = {"ascpp", "-o-18446744073709551616"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=non-int"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=12non-int"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=non-int12"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
}

TEST(TestCmdline, FloatOption) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<float>('o', "opt", "float option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5);

  args = {"ascpp", "-o", "0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5);

  args = {"ascpp", "--opt="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0);

  args = {"ascpp", "--opt=1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 1);

  args = {"ascpp", "--opt=0."};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.);

  args = {"ascpp", "--opt=.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), .5);

  args = {"ascpp", "--opt=0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5);

  args = {"ascpp", "--opt=1e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 1e1);

  args = {"ascpp", "--opt=0.e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.e1);

  args = {"ascpp", "--opt=.5e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), .5e1);

  args = {"ascpp", "--opt=0.5e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5e1);

  args = {"ascpp", "--opt=1e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 1e+1);

  args = {"ascpp", "--opt=0.e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.e+1);

  args = {"ascpp", "--opt=.5e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), .5e+1);

  args = {"ascpp", "--opt=0.5e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5e+1);

  args = {"ascpp", "--opt=1e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 1e-1);

  args = {"ascpp", "--opt=0.e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.e-1);

  args = {"ascpp", "--opt=.5e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), .5e-1);

  args = {"ascpp", "--opt=0.5e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.GetValue<float>("opt"), 0.5e-1);

  args = {"ascpp", "--opt=3.402823466e+38"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<float>("opt"), std::numeric_limits<float>::max());

  args = {"ascpp", "--opt=-3.402823466e+38"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<float>("opt"), std::numeric_limits<float>::lowest());

  args = {"ascpp", "--opt=1.175494351e-38"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<float>("opt"), std::numeric_limits<float>::min());

  args = {"ascpp", "--opt=3.402823466e+39"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
  args = {"ascpp", "--opt=-3.402823466e+39"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
  args = {"ascpp", "--opt=1.175494351e-50"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=0.0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<float>("opt"), 0.0);

  args = {"ascpp", "--opt=+0.0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<float>("opt"), 0.0);

  args = {"ascpp", "--opt=-0.0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<float>("opt"), 0.0);

  args = {"ascpp", "--opt=NAN"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_TRUE(std::isnan(cmd.GetValue<float>("opt")));

  args = {"ascpp", "--opt=+inf"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.GetValue<float>("opt")));
  EXPECT_EQ(cmd.GetValue<float>("opt"), std::numeric_limits<float>::infinity());

  args = {"ascpp", "--opt=-inf"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.GetValue<float>("opt")));
  EXPECT_EQ(cmd.GetValue<float>("opt"), -std::numeric_limits<float>::infinity());
}

TEST(TestCmdline, DoubleOption) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<double>('o', "opt", "double option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5);

  args = {"ascpp", "-o", "0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt", "0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5);

  args = {"ascpp", "--opt="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0);

  args = {"ascpp", "--opt=1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 1);

  args = {"ascpp", "--opt=0."};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.);

  args = {"ascpp", "--opt=.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), .5);

  args = {"ascpp", "--opt=0.5"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5);

  args = {"ascpp", "--opt=1e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 1e1);

  args = {"ascpp", "--opt=0.e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.e1);

  args = {"ascpp", "--opt=.5e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), .5e1);

  args = {"ascpp", "--opt=0.5e1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5e1);

  args = {"ascpp", "--opt=1e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 1e+1);

  args = {"ascpp", "--opt=0.e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.e+1);

  args = {"ascpp", "--opt=.5e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), .5e+1);

  args = {"ascpp", "--opt=0.5e+1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5e+1);

  args = {"ascpp", "--opt=1e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 1e-1);

  args = {"ascpp", "--opt=0.e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.e-1);

  args = {"ascpp", "--opt=.5e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), .5e-1);

  args = {"ascpp", "--opt=0.5e-1"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.GetValue<double>("opt"), 0.5e-1);

  args = {"ascpp", "--opt=+1.7976931348623158e+308"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<double>("opt"), std::numeric_limits<double>::max());

  args = {"ascpp", "--opt=-1.7976931348623158e+308"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<double>("opt"), std::numeric_limits<double>::lowest());

  args = {"ascpp", "--opt=2.2250738585072014e-308"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<double>("opt"), std::numeric_limits<double>::min());

  args = {"ascpp", "--opt=+1.7976931348623159e+308"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
  args = {"ascpp", "--opt=+1.7976931348623158e+309"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
  args = {"ascpp", "--opt=-1.7976931348623159e+308"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
  args = {"ascpp", "--opt=-1.7976931348623158e+309"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));
  args = {"ascpp", "--opt=2.2250738585072013e-400"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "--opt=0.0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<double>("opt"), 0.0);

  args = {"ascpp", "--opt=+0.0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<double>("opt"), 0.0);

  args = {"ascpp", "--opt=-0.0"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<double>("opt"), 0.0);

  args = {"ascpp", "--opt=NAN"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_TRUE(std::isnan(cmd.GetValue<double>("opt")));

  args = {"ascpp", "--opt=+inf"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.GetValue<double>("opt")));
  EXPECT_EQ(cmd.GetValue<double>("opt"), std::numeric_limits<double>::infinity());

  args = {"ascpp", "--opt=-inf"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.GetValue<double>("opt")));
  EXPECT_EQ(cmd.GetValue<double>("opt"), -std::numeric_limits<double>::infinity());
}

TEST(TestCmdline, StringOption) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<std::string>('o', "opt", "string option");

  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.ParseArgs(args.size(), args.data()));

  args = {"ascpp", "-ostring"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("opt"), "string");

  args = {"ascpp", "-o", "string"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("opt"), "string");

  args = {"ascpp", "--opt", "string"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("opt"), "string");

  args = {"ascpp", "--opt=string"};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("opt"), "string");

  args = {"ascpp", "--opt="};
  cmd.ParseArgs(args.size(), args.data());
  EXPECT_EQ(cmd.GetValue<std::string>("opt"), "");
}

TEST(TestCmdline, MultiOpt) {
  auto cmd = ascpp::Cmdline(&app_info);
  auto args = std::vector<const char*>();

  cmd.AddOption<std::vector<bool>>('b', "bool", "bool option");
  cmd.AddOption<std::vector<int>>('i', "int", "int option");
  cmd.AddOption<std::vector<size_t>>('z', "size_t", "size_t option");
  cmd.AddOption<std::vector<float>>('f', "float", "float option");
  cmd.AddOption<std::vector<double>>('d', "double", "double option");
  cmd.AddOption<std::vector<std::string>>('s', "string", "string option");

  args = {"ascpp", "-b1", "-i1,", "-z,", "-f,1", "-d1,,", "-s,,1"};
  cmd.ParseArgs(args.size(), args.data());
  auto vb = std::vector<bool>{true};
  EXPECT_EQ(cmd.GetValue<std::vector<bool>>("b"), vb);
  auto vi = std::vector<int>{1};
  EXPECT_EQ(cmd.GetValue<std::vector<int>>("i"), vi);
  auto vz = std::vector<size_t>{0};
  EXPECT_EQ(cmd.GetValue<std::vector<size_t>>("z"), vz);
  auto vf = std::vector<float>{0, 1};
  EXPECT_EQ(cmd.GetValue<std::vector<float>>("f"), vf);
  auto vd = std::vector<double>{1, 0};
  EXPECT_EQ(cmd.GetValue<std::vector<double>>("d"), vd);
  auto vs = std::vector<std::string>{"", "", "1"};
  EXPECT_EQ(cmd.GetValue<std::vector<std::string>>("s"), vs);
}

