#include "utils/cmdline.hpp"

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

// NOLINTBEGIN(modernize-use-trailing-return-type,bugprone-narrowing-conversions)

TEST(TestCmdline, GetType) {
  EXPECT_EQ(ascpp::option::get_type<bool>(), ascpp::option::S_BOOL);
  EXPECT_EQ(ascpp::option::get_type<int>(), ascpp::option::S_INT);
  EXPECT_EQ(ascpp::option::get_type<size_t>(), ascpp::option::S_SIZE);
  EXPECT_EQ(ascpp::option::get_type<float>(), ascpp::option::S_FLOAT);
  EXPECT_EQ(ascpp::option::get_type<double>(), ascpp::option::S_DOUBLE);
  EXPECT_EQ(ascpp::option::get_type<std::string>(), ascpp::option::S_STRING);
  EXPECT_EQ(ascpp::option::get_type<std::vector<bool>>(), ascpp::option::M_BOOL);
  EXPECT_EQ(ascpp::option::get_type<std::vector<int>>(), ascpp::option::M_INT);
  EXPECT_EQ(ascpp::option::get_type<std::vector<size_t>>(), ascpp::option::M_SIZE);
  EXPECT_EQ(ascpp::option::get_type<std::vector<float>>(), ascpp::option::M_FLOAT);
  EXPECT_EQ(ascpp::option::get_type<std::vector<double>>(), ascpp::option::M_DOUBLE);
  EXPECT_EQ(ascpp::option::get_type<std::vector<std::string>>(), ascpp::option::M_STRING);
}

TEST(TestCmdline, TypeToStr) {
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::S_BOOL), "bool");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::S_INT), "int");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::S_SIZE), "size_t");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::S_FLOAT), "float");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::S_DOUBLE), "double");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::S_STRING), "string");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::M_BOOL), "list of bool");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::M_INT), "list of int");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::M_SIZE), "list of size_t");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::M_FLOAT), "list of float");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::M_DOUBLE), "list of double");
  EXPECT_EQ(ascpp::option::type_to_str(ascpp::option::M_STRING), "list of string");
}

TEST(TestCmdline, CheckValue) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  auto optadder = cmd.add_option<int>('i', "int", "int option")
                      .with_limits([](int i) { return i % 2; })
                      .with_default(1);
  args = {"ascpp", "-i0"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "-i1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("i"), 1);
  args = {"ascpp", "-i2"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  optadder.with_limits({0, 1, 1, 2, 3, 5});
  args = {"ascpp", "-i0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("i"), 0);
  args = {"ascpp", "-i1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("i"), 1);
  args = {"ascpp", "-i2"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("i"), 2);
  args = {"ascpp", "-i3"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("i"), 3);
  args = {"ascpp", "-i4"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "-i5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("i"), 5);
  args = {"ascpp", "-i6"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  auto optadder2
      = cmd.add_option<std::vector<int>>('I', "Int", "int option").with_limits([](int i) {
          return i % 2;
        });
  auto v = std::vector<int>{};
  args = {"ascpp", "-I0,2,4,6,8,10"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "-I1,3,5,7,9"};
  cmd.parse_args(args.size(), args.data());
  v = {1, 3, 5, 7, 9};
  EXPECT_EQ(cmd.get_value<std::vector<int>>("I"), v);
  args = {"ascpp", "-I1,2"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  optadder2.with_limits({0, 1, 1, 2, 3, 5});
  args = {"ascpp", "-I-1,4,6"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "-I0,1,2,3"};
  cmd.parse_args(args.size(), args.data());
  v = {0, 1, 2, 3};
  EXPECT_EQ(cmd.get_value<std::vector<int>>("I"), v);
  args = {"ascpp", "-I0,3,4"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
}

TEST(TestCmdline, AddSingleOptions) {
  auto cmd = ascpp::cmdline(&info);
  cmd.add_option<bool>('b', "bool", "bool desc");

  auto& b = cmd.get_option('b');
  auto& bol = cmd.get_option("bool");
  EXPECT_EQ(&b, &bol);
  EXPECT_EQ(bol.opt_type, ascpp::option::S_BOOL);
  EXPECT_EQ(bol.short_opt, "b");
  EXPECT_EQ(bol.long_opt, "bool");
  EXPECT_EQ(bol.opt_desc, "bool desc");
  EXPECT_EQ(bol.limits.has_value(), false);
  EXPECT_EQ(std::any_cast<bool>(bol.default_value), false);
  EXPECT_EQ(std::any_cast<bool>(bol.implicit_value), true);

  cmd.add_option<int>("int", "int desc");
  auto& intt = cmd.get_option("int");
  EXPECT_EQ(intt.opt_type, ascpp::option::S_INT);
  EXPECT_EQ(intt.short_opt, "");
  EXPECT_EQ(intt.long_opt, "int");
  EXPECT_EQ(intt.opt_desc, "int desc");
  EXPECT_EQ(intt.limits.has_value(), false);
  EXPECT_EQ(intt.default_value.has_value(), false);
  EXPECT_EQ(intt.implicit_value.has_value(), false);

  auto limit_set_adder = cmd.add_option<size_t>("size_t", "size_t desc").with_limits({0, 1, 2});
  auto& size = cmd.get_option("size_t");
  EXPECT_EQ(size.opt_type, ascpp::option::S_SIZE);
  EXPECT_EQ(size.short_opt, "");
  EXPECT_EQ(size.long_opt, "size_t");
  EXPECT_EQ(size.opt_desc, "size_t desc");
  auto size_set = std::unordered_set<size_t>{0, 1, 2};
  EXPECT_EQ(std::any_cast<const std::unordered_set<size_t>&>(size.limits), size_set);
  EXPECT_EQ(size.default_value.has_value(), false);
  EXPECT_EQ(size.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_set_adder.with_default(3));
  EXPECT_ANY_THROW(limit_set_adder.with_implicit(-1));
  limit_set_adder.with_default(0);
  EXPECT_EQ(std::any_cast<size_t>(size.default_value), 0);
  limit_set_adder.with_implicit(1);
  EXPECT_EQ(std::any_cast<size_t>(size.implicit_value), 1);

  auto limit_pred_adder
      = cmd.add_option<float>("float", "float desc").with_limits([](auto v) { return v > 0; });
  auto& flt = cmd.get_option("float");
  EXPECT_EQ(flt.opt_type, ascpp::option::S_FLOAT);
  EXPECT_EQ(flt.short_opt, "");
  EXPECT_EQ(flt.long_opt, "float");
  EXPECT_EQ(flt.opt_desc, "float desc");
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(1), true);
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(0), false);
  EXPECT_EQ(flt.default_value.has_value(), false);
  EXPECT_EQ(flt.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_pred_adder.with_default(-1));
  EXPECT_ANY_THROW(limit_pred_adder.with_implicit(0));
  limit_pred_adder.with_default(1);
  EXPECT_EQ(std::any_cast<float>(flt.default_value), 1);
  limit_pred_adder.with_implicit(2);
  EXPECT_EQ(std::any_cast<float>(flt.implicit_value), 2);

  cmd.add_option<double>("double", "double desc").with_default(1.5);
  auto& dbl = cmd.get_option("double");
  EXPECT_EQ(dbl.opt_type, ascpp::option::S_DOUBLE);
  EXPECT_EQ(dbl.short_opt, "");
  EXPECT_EQ(dbl.long_opt, "double");
  EXPECT_EQ(dbl.opt_desc, "double desc");
  EXPECT_EQ(dbl.limits.has_value(), false);
  EXPECT_DOUBLE_EQ(std::any_cast<double>(dbl.default_value), 1.5);
  EXPECT_EQ(dbl.implicit_value.has_value(), false);

  cmd.add_option<std::string>("string", "string desc").with_implicit("str");
  auto& str = cmd.get_option("string");
  EXPECT_EQ(str.opt_type, ascpp::option::S_STRING);
  EXPECT_EQ(str.short_opt, "");
  EXPECT_EQ(str.long_opt, "string");
  EXPECT_EQ(str.opt_desc, "string desc");
  EXPECT_EQ(str.limits.has_value(), false);
  EXPECT_EQ(str.default_value.has_value(), false);
  EXPECT_EQ(std::any_cast<std::string>(str.implicit_value), "str");

  std::clog << cmd.help_string() << std::endl;
}

TEST(TestCmdline, AddMultiOptions) {
  auto cmd = ascpp::cmdline(&info);
  cmd.add_option<std::vector<bool>>('b', "bool", "bool desc");

  auto& b = cmd.get_option('b');
  auto& bol = cmd.get_option("bool");
  EXPECT_EQ(&b, &bol);
  EXPECT_EQ(bol.opt_type, ascpp::option::M_BOOL);
  EXPECT_EQ(bol.short_opt, "b");
  EXPECT_EQ(bol.long_opt, "bool");
  EXPECT_EQ(bol.opt_desc, "bool desc");
  EXPECT_EQ(bol.limits.has_value(), false);
  EXPECT_EQ(bol.default_value.has_value(), false);
  EXPECT_EQ(bol.implicit_value.has_value(), false);

  cmd.add_option<std::vector<int>>("int", "int desc");
  auto& intt = cmd.get_option("int");
  EXPECT_EQ(intt.opt_type, ascpp::option::M_INT);
  EXPECT_EQ(intt.short_opt, "");
  EXPECT_EQ(intt.long_opt, "int");
  EXPECT_EQ(intt.opt_desc, "int desc");
  EXPECT_EQ(intt.limits.has_value(), false);
  EXPECT_EQ(intt.default_value.has_value(), false);
  EXPECT_EQ(intt.implicit_value.has_value(), false);

  auto limit_set_adder
      = cmd.add_option<std::vector<size_t>>("size_t", "size_t desc").with_limits({0, 1, 2});
  auto& size = cmd.get_option("size_t");
  EXPECT_EQ(size.opt_type, ascpp::option::M_SIZE);
  EXPECT_EQ(size.short_opt, "");
  EXPECT_EQ(size.long_opt, "size_t");
  EXPECT_EQ(size.opt_desc, "size_t desc");
  auto size_set = std::unordered_set<size_t>{0, 1, 2};
  EXPECT_EQ(std::any_cast<const std::unordered_set<size_t>&>(size.limits), size_set);
  EXPECT_EQ(size.default_value.has_value(), false);
  EXPECT_EQ(size.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_set_adder.with_default({3}));
  EXPECT_ANY_THROW(limit_set_adder.with_implicit({2, 3}));
  auto size_vec = std::vector<size_t>{1};
  limit_set_adder.with_default({1});
  EXPECT_EQ(std::any_cast<const std::vector<size_t>&>(size.default_value), size_vec);
  size_vec = {0, 1, 2};
  limit_set_adder.with_implicit({0, 1, 2});
  EXPECT_EQ(std::any_cast<const std::vector<size_t>&>(size.implicit_value), size_vec);

  auto limit_pred_adder
      = cmd.add_option<std::vector<float>>("float", "float desc").with_limits([](auto v) {
          return v > 0;
        });
  auto& flt = cmd.get_option("float");
  EXPECT_EQ(flt.opt_type, ascpp::option::M_FLOAT);
  EXPECT_EQ(flt.short_opt, "");
  EXPECT_EQ(flt.long_opt, "float");
  EXPECT_EQ(flt.opt_desc, "float desc");
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(1), true);
  EXPECT_EQ(std::any_cast<const std::function<bool(const float&)>&>(flt.limits)(0), false);
  EXPECT_EQ(flt.default_value.has_value(), false);
  EXPECT_EQ(flt.implicit_value.has_value(), false);

  EXPECT_ANY_THROW(limit_pred_adder.with_default({0, 1}));
  EXPECT_ANY_THROW(limit_pred_adder.with_implicit({-1}));
  limit_pred_adder.with_default({1}).with_implicit({2, 3});
  ;
  auto flt_vec = std::vector<float>{1};
  EXPECT_EQ(std::any_cast<const std::vector<float>&>(flt.default_value), flt_vec);
  flt_vec = {2, 3};
  EXPECT_EQ(std::any_cast<const std::vector<float>&>(flt.implicit_value), flt_vec);

  cmd.add_option<std::vector<double>>("double", "double desc").with_default({1.5});
  auto& dbl = cmd.get_option("double");
  EXPECT_EQ(dbl.opt_type, ascpp::option::M_DOUBLE);
  EXPECT_EQ(dbl.short_opt, "");
  EXPECT_EQ(dbl.long_opt, "double");
  EXPECT_EQ(dbl.opt_desc, "double desc");
  EXPECT_EQ(dbl.limits.has_value(), false);
  auto dbl_vec = std::vector<double>{1.5};
  EXPECT_EQ(std::any_cast<const std::vector<double>&>(dbl.default_value), dbl_vec);
  EXPECT_EQ(dbl.implicit_value.has_value(), false);

  cmd.add_option<std::vector<std::string>>("string", "string desc").with_implicit({"str"});
  auto& str = cmd.get_option("string");
  EXPECT_EQ(str.opt_type, ascpp::option::M_STRING);
  EXPECT_EQ(str.short_opt, "");
  EXPECT_EQ(str.long_opt, "string");
  EXPECT_EQ(str.opt_desc, "string desc");
  EXPECT_EQ(str.limits.has_value(), false);
  EXPECT_EQ(str.default_value.has_value(), false);
  auto str_vec = std::vector<std::string>{"str"};
  EXPECT_EQ(std::any_cast<const std::vector<std::string>&>(str.implicit_value), str_vec);

  std::clog << cmd.help_string() << std::endl;
}

TEST(TestCmdline, AddBadOptions) {
  auto cmd = ascpp::cmdline{&info};
  cmd.add_option<bool>('o', "option", "normal option");

  EXPECT_ANY_THROW(cmd.add_option<bool>("option", "duplicate long option name"));
  EXPECT_ANY_THROW(cmd.add_option<bool>('o', "longoption", "duplicate short option name"));

  EXPECT_ANY_THROW(cmd.add_option<bool>('\0', "zero", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>('\n', "newline", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>('\t', "tab", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>(' ', "space", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>('-', "hyphen", ""));
  cmd.add_option<bool>('=', "equal", "allowed = in short option");
  EXPECT_EQ(cmd.get_option('=').short_opt, "=");
  EXPECT_EQ(cmd.get_option('=').long_opt, "equal");

  EXPECT_ANY_THROW(cmd.add_option<bool>("1", "too short, at least two char"));
  EXPECT_ANY_THROW(cmd.add_option<bool>("with\nnewline", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>("with\ttab", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>("with space", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>("with=", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>("=with", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>("wi=th", ""));
  EXPECT_ANY_THROW(cmd.add_option<bool>("中文选项", ""));
  cmd.add_option<bool>("-hyphen", "allowed - in long option");
  EXPECT_EQ(cmd.get_option("-hyphen").short_opt, "");
  EXPECT_EQ(cmd.get_option("-hyphen").long_opt, "-hyphen");
  cmd.add_option<bool>("hyphen-", "allowed - in long option");
  EXPECT_EQ(cmd.get_option("hyphen-").short_opt, "");
  EXPECT_EQ(cmd.get_option("hyphen-").long_opt, "hyphen-");
  cmd.add_option<bool>("hy-phen", "allowed - in long option");
  EXPECT_EQ(cmd.get_option("hy-phen").short_opt, "");
  EXPECT_EQ(cmd.get_option("hy-phen").long_opt, "hy-phen");
}

TEST(TestCmdline, BasicParse) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<bool>('n', "non", "bool short option requires no value");
  cmd.add_option<std::string>('o', "optional",
                              "short option with implicit value optional need a value")
      .with_default("default")
      .with_implicit("implicit");
  cmd.add_option<std::string>('r', "required",
                              "short option without implicit value requires a value")
      .with_default("default");
  cmd.allow_nonoptions("nonopts", false);

  args = {"ascpp"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), false);
  EXPECT_EQ(cmd.get_value<std::string>("o"), "default");
  EXPECT_EQ(cmd.get_value<std::string>("r"), "default");

  args = {"ascpp", "-VALUE"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-n"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);

  args = {"ascpp", "-nVALUE"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-n", "VALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-o"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "implicit");

  args = {"ascpp", "-oVALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "VALUE");

  args = {"ascpp", "-o", "VALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-r"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-rVALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "VALUE");

  args = {"ascpp", "-r", "-n"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "-n");

  args = {"ascpp", "-r", "--non"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "--non");

  args = {"ascpp", "-noVALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("o"), "VALUE");

  args = {"ascpp", "-no", "VALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-nrVALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("r"), "VALUE");

  args = {"ascpp", "-nr", "-VALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("r"), "-VALUE");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{});

  args = {"ascpp", "-norVALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("o"), "rVALUE");
  EXPECT_EQ(cmd.get_value<std::string>("r"), "default");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{});

  args = {"ascpp", "-nor", "VALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("o"), "r");
  EXPECT_EQ(cmd.get_value<std::string>("r"), "default");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "-nroVALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("r"), "oVALUE");
  EXPECT_EQ(cmd.get_value<std::string>("o"), "default");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{});

  args = {"ascpp", "-nro", "VALUE"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_value<std::string>("o"), "default");
  EXPECT_EQ(cmd.get_value<std::string>("r"), "o");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"VALUE"});

  args = {"ascpp", "--non"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);

  args = {"ascpp", "--non="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), false);

  args = {"ascpp", "--non=false"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), false);

  args = {"ascpp", "--non", "true"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("n"), true);
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"true"});

  args = {"ascpp", "--optional"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "implicit");

  args = {"ascpp", "--optional="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "");

  args = {"ascpp", "--optional=false"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "false");

  args = {"ascpp", "--optional", "true"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"true"});

  args = {"ascpp", "--required"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--required="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "");

  args = {"ascpp", "--required==false"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "=false");

  args = {"ascpp", "--required", "--non"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "--non");

  args = {"ascpp", "--required", "-n"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "-n");

  args = {"ascpp", "0",         "-r", "true", "1",     "-o",      "2",
          "--",    "all after", "--", "are",  "--non", "-option", "arguments"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("r"), "true");
  EXPECT_EQ(cmd.get_value<std::string>("o"), "implicit");
  EXPECT_EQ(cmd.get_value<bool>("n"), false);
  auto nonoptions = std::vector<std::string>{"0",   "1",     "2",       "all after", "--",
                                             "are", "--non", "-option", "arguments"};
  EXPECT_EQ(cmd.get_nonoptions(), nonoptions);

  std::clog << cmd.help_string() << std::endl;
}

TEST(TestCmdline, BoolOption) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<bool>('o', "opt", "bool option");
  cmd.allow_nonoptions("nonopts", false);

  args = {"ascpp"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), false);

  args = {"ascpp", "-o"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);

  args = {"ascpp", "-o", "false"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"false"});

  args = {"ascpp", "--opt"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);

  args = {"ascpp", "--opt", "false"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);
  EXPECT_EQ(cmd.get_nonoptions(), std::vector<std::string>{"false"});

  args = {"ascpp", "--opt="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), false);

  args = {"ascpp", "--opt=true"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);

  args = {"ascpp", "--opt=false"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), false);

  args = {"ascpp", "--opt=yes"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);

  args = {"ascpp", "--opt=no"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), false);

  args = {"ascpp", "--opt=on"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);

  args = {"ascpp", "--opt=off"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), false);

  args = {"ascpp", "--opt=1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), true);

  args = {"ascpp", "--opt=0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<bool>("opt"), false);

  args = {"ascpp", "--opt=other"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=2"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=-1"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
}

TEST(TestCmdline, IntOption) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<int>('o', "opt", "int option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 1);

  args = {"ascpp", "-o", "1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 1);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt", "1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 1);

  args = {"ascpp", "--opt="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 0);

  args = {"ascpp", "--opt=0b10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 2);

  args = {"ascpp", "--opt=0B10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 2);

  args = {"ascpp", "--opt=0o10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 8);

  args = {"ascpp", "--opt=0O10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 8);

  args = {"ascpp", "--opt=010"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 8);

  args = {"ascpp", "--opt=0x10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 16);

  args = {"ascpp", "--opt=0x10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 16);

  args = {"ascpp", "--opt=0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 0);

  args = {"ascpp", "--opt=+0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 0);

  args = {"ascpp", "--opt=-0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), 0);

  args = {"ascpp", "--opt=+2147483647"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), std::numeric_limits<int>::max());

  args = {"ascpp", "--opt=+2147483648"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=-2147483648"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<int>("opt"), std::numeric_limits<int>::min());

  args = {"ascpp", "--opt=-2147483649"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=not-number"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=12not-number"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=not-number12"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
}

TEST(TestCmdline, SizeOption) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<size_t>('o', "opt", "size_t option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 1);

  args = {"ascpp", "-o", "1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 1);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt", "1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 1);

  args = {"ascpp", "--opt="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 0);

  args = {"ascpp", "--opt=0b10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 2);

  args = {"ascpp", "--opt=0B10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 2);

  args = {"ascpp", "--opt=0o10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 8);

  args = {"ascpp", "--opt=0O10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 8);

  args = {"ascpp", "--opt=010"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 8);

  args = {"ascpp", "--opt=0x10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 16);

  args = {"ascpp", "--opt=0x10"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 16);

  args = {"ascpp", "--opt=0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 0);

  args = {"ascpp", "--opt=+0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 0);

  args = {"ascpp", "--opt=-0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 0);

  args = {"ascpp", "-o18446744073709551615"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), std::numeric_limits<size_t>::max());

  args = {"ascpp", "-o18446744073709551616"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  // unsigned integer wraparound rules
  args = {"ascpp", "-o-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), std::numeric_limits<size_t>::max());

  args = {"ascpp", "-o-18446744073709551615"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<size_t>("opt"), 1);

  args = {"ascpp", "-o-18446744073709551616"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=non-int"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=12non-int"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=non-int12"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
}

TEST(TestCmdline, FloatOption) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<float>('o', "opt", "float option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5);

  args = {"ascpp", "-o", "0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt", "0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5);

  args = {"ascpp", "--opt="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0);

  args = {"ascpp", "--opt=1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 1);

  args = {"ascpp", "--opt=0."};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.);

  args = {"ascpp", "--opt=.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), .5);

  args = {"ascpp", "--opt=0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5);

  args = {"ascpp", "--opt=1e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 1e1);

  args = {"ascpp", "--opt=0.e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.e1);

  args = {"ascpp", "--opt=.5e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), .5e1);

  args = {"ascpp", "--opt=0.5e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5e1);

  args = {"ascpp", "--opt=1e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 1e+1);

  args = {"ascpp", "--opt=0.e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.e+1);

  args = {"ascpp", "--opt=.5e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), .5e+1);

  args = {"ascpp", "--opt=0.5e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5e+1);

  args = {"ascpp", "--opt=1e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 1e-1);

  args = {"ascpp", "--opt=0.e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.e-1);

  args = {"ascpp", "--opt=.5e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), .5e-1);

  args = {"ascpp", "--opt=0.5e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_FLOAT_EQ(cmd.get_value<float>("opt"), 0.5e-1);

  args = {"ascpp", "--opt=3.402823466e+38"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<float>("opt"), std::numeric_limits<float>::max());

  args = {"ascpp", "--opt=-3.402823466e+38"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<float>("opt"), std::numeric_limits<float>::lowest());

  args = {"ascpp", "--opt=1.175494351e-38"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<float>("opt"), std::numeric_limits<float>::min());

  args = {"ascpp", "--opt=3.402823466e+39"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "--opt=-3.402823466e+39"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "--opt=1.175494351e-50"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=0.0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<float>("opt"), 0.0);

  args = {"ascpp", "--opt=+0.0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<float>("opt"), 0.0);

  args = {"ascpp", "--opt=-0.0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<float>("opt"), 0.0);

  args = {"ascpp", "--opt=NAN"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_TRUE(std::isnan(cmd.get_value<float>("opt")));

  args = {"ascpp", "--opt=+inf"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.get_value<float>("opt")));
  EXPECT_EQ(cmd.get_value<float>("opt"), std::numeric_limits<float>::infinity());

  args = {"ascpp", "--opt=-inf"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.get_value<float>("opt")));
  EXPECT_EQ(cmd.get_value<float>("opt"), -std::numeric_limits<float>::infinity());
}

TEST(TestCmdline, DoubleOption) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<double>('o', "opt", "double option");
  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5);

  args = {"ascpp", "-o", "0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5);

  args = {"ascpp", "--opt"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt", "0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5);

  args = {"ascpp", "--opt="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0);

  args = {"ascpp", "--opt=1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 1);

  args = {"ascpp", "--opt=0."};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.);

  args = {"ascpp", "--opt=.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), .5);

  args = {"ascpp", "--opt=0.5"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5);

  args = {"ascpp", "--opt=1e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 1e1);

  args = {"ascpp", "--opt=0.e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.e1);

  args = {"ascpp", "--opt=.5e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), .5e1);

  args = {"ascpp", "--opt=0.5e1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5e1);

  args = {"ascpp", "--opt=1e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 1e+1);

  args = {"ascpp", "--opt=0.e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.e+1);

  args = {"ascpp", "--opt=.5e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), .5e+1);

  args = {"ascpp", "--opt=0.5e+1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5e+1);

  args = {"ascpp", "--opt=1e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 1e-1);

  args = {"ascpp", "--opt=0.e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.e-1);

  args = {"ascpp", "--opt=.5e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), .5e-1);

  args = {"ascpp", "--opt=0.5e-1"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_DOUBLE_EQ(cmd.get_value<double>("opt"), 0.5e-1);

  args = {"ascpp", "--opt=+1.7976931348623158e+308"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<double>("opt"), std::numeric_limits<double>::max());

  args = {"ascpp", "--opt=-1.7976931348623158e+308"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<double>("opt"), std::numeric_limits<double>::lowest());

  args = {"ascpp", "--opt=2.2250738585072014e-308"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<double>("opt"), std::numeric_limits<double>::min());

  args = {"ascpp", "--opt=+1.7976931348623159e+308"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "--opt=+1.7976931348623158e+309"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "--opt=-1.7976931348623159e+308"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "--opt=-1.7976931348623158e+309"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));
  args = {"ascpp", "--opt=2.2250738585072013e-400"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "--opt=0.0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<double>("opt"), 0.0);

  args = {"ascpp", "--opt=+0.0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<double>("opt"), 0.0);

  args = {"ascpp", "--opt=-0.0"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<double>("opt"), 0.0);

  args = {"ascpp", "--opt=NAN"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_TRUE(std::isnan(cmd.get_value<double>("opt")));

  args = {"ascpp", "--opt=+inf"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.get_value<double>("opt")));
  EXPECT_EQ(cmd.get_value<double>("opt"), std::numeric_limits<double>::infinity());

  args = {"ascpp", "--opt=-inf"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_TRUE(std::isinf(cmd.get_value<double>("opt")));
  EXPECT_EQ(cmd.get_value<double>("opt"), -std::numeric_limits<double>::infinity());
}

TEST(TestCmdline, StringOption) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<std::string>('o', "opt", "string option");

  args = {"ascpp"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-o"};
  EXPECT_ANY_THROW(cmd.parse_args(args.size(), args.data()));

  args = {"ascpp", "-ostring"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("opt"), "string");

  args = {"ascpp", "-o", "string"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("opt"), "string");

  args = {"ascpp", "--opt", "string"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("opt"), "string");

  args = {"ascpp", "--opt=string"};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("opt"), "string");

  args = {"ascpp", "--opt="};
  cmd.parse_args(args.size(), args.data());
  EXPECT_EQ(cmd.get_value<std::string>("opt"), "");
}

TEST(TestCmdline, MultiOpt) {
  auto cmd = ascpp::cmdline(&info);
  auto args = std::vector<const char*>();

  cmd.add_option<std::vector<bool>>('b', "bool", "bool option");
  cmd.add_option<std::vector<int>>('i', "int", "int option");
  cmd.add_option<std::vector<size_t>>('z', "size_t", "size_t option");
  cmd.add_option<std::vector<float>>('f', "float", "float option");
  cmd.add_option<std::vector<double>>('d', "double", "double option");
  cmd.add_option<std::vector<std::string>>('s', "string", "string option");

  args = {"ascpp", "-b1", "-i1,", "-z,", "-f,1", "-d1,,", "-s,,1"};
  cmd.parse_args(args.size(), args.data());
  auto vb = std::vector<bool>{true};
  EXPECT_EQ(cmd.get_value<std::vector<bool>>("b"), vb);
  auto vi = std::vector<int>{1, 0};
  EXPECT_EQ(cmd.get_value<std::vector<int>>("i"), vi);
  auto vz = std::vector<size_t>{0, 0};
  EXPECT_EQ(cmd.get_value<std::vector<size_t>>("z"), vz);
  auto vf = std::vector<float>{0, 1};
  EXPECT_EQ(cmd.get_value<std::vector<float>>("f"), vf);
  auto vd = std::vector<double>{1, 0, 0};
  EXPECT_EQ(cmd.get_value<std::vector<double>>("d"), vd);
  auto vs = std::vector<std::string>{"", "", "1"};
  EXPECT_EQ(cmd.get_value<std::vector<std::string>>("s"), vs);

  cmd = ascpp::cmdline(&info);
  cmd.add_option<std::vector<std::string>>('s', "string", "string option");
  args = {"ascpp", "-s,1,2,3,,"};
  cmd.parse_args(args.size(), args.data());
  vs = {"", "1", "2", "3", "", ""};
  EXPECT_EQ(cmd.get_value<std::vector<std::string>>("s"), vs);
}

// NOLINTEND(modernize-use-trailing-return-type,bugprone-narrowing-conversions)
