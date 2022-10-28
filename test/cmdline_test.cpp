#include "utils/cmdline.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "app.hpp"

namespace ascpp {

TEST(TestCmdline, BasicUsage) {
  ascpp::Cmdline cmdline{&app_info};
  cmdline.AddOption<std::string>('s', "str", "");
  std::vector<const char*> argv{};
  std::vector<std::string> unmatched{};

  argv = {"ascpp", "-shello"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "hello");

  argv = {"ascpp", "-s", "hello"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "hello");

  argv = {"ascpp", "--str=hello"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "hello");

  argv = {"ascpp", "--str=hello", "world"};
  unmatched = {"world"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline.GetUnmatched(), unmatched);

  argv = {"ascpp", "--str=hello", "--str=world"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "world");

  argv = {"ascpp", "--str=hello", "--", "--str=world"};
  unmatched = {"--str=world"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline.GetUnmatched(), unmatched);
}

TEST(TestCmdline, AddBoolOption) {
  ascpp::Cmdline cmdline{&app_info};
  cmdline.AddOption<bool>('b', "short_b", "");
  cmdline.AddOption<bool>("bool", "");
  ascpp::Cmdline cmdline_default{&app_info};
  cmdline_default.AddOptionWithDefault<bool>('b', "short_b", "", true);
  cmdline_default.AddOptionWithDefault<bool>("bool", "", true);
  ascpp::Cmdline cmdline_implicit{&app_info};
  cmdline_implicit.AddOptionWithImplicit<bool>('b', "short_b", "", false);
  cmdline_implicit.AddOptionWithImplicit<bool>("bool", "", false);
  ascpp::Cmdline cmdline_both{&app_info};
  cmdline_both.AddOptionWithBoth<bool>('b', "short_b", "", true, false);
  cmdline_both.AddOptionWithBoth<bool>("bool", "", true, false);
  std::vector<const char*> argv{};

  // bool has default default_value: false
  argv = {"ascpp"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("b"), false);
  EXPECT_EQ(cmdline.Get<bool>("short_b"), false);
  EXPECT_EQ(cmdline.Get<bool>("bool"), false);

  // set default_value to true
  argv = {"ascpp"};
  cmdline_default.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.Get<bool>("b"), true);
  EXPECT_EQ(cmdline_default.Get<bool>("short_b"), true);
  EXPECT_EQ(cmdline_default.Get<bool>("bool"), true);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<bool>("b"), true);
  EXPECT_EQ(cmdline_both.Get<bool>("short_b"), true);
  EXPECT_EQ(cmdline_both.Get<bool>("bool"), true);

  // bool has default implicit_value: true
  argv = {"ascpp", "-b"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("b"), true);
  EXPECT_EQ(cmdline.Get<bool>("short_b"), true);
  argv = {"ascpp", "--bool"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("bool"), true);

  // set implicit_value to false
  argv = {"ascpp", "-b"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<bool>("b"), false);
  EXPECT_EQ(cmdline_implicit.Get<bool>("short_b"), false);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<bool>("b"), false);
  EXPECT_EQ(cmdline_both.Get<bool>("short_b"), false);
  argv = {"ascpp", "--bool"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<bool>("bool"), false);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<bool>("bool"), false);

  // unable to set the value of short options that has implicit_value
  argv = {"ascpp", "-btrue"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "-bfalse"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "-b", "true"};
  EXPECT_EQ(cmdline.Get<bool>("bool"), true);
  argv = {"ascpp", "-b", "false"};
  EXPECT_EQ(cmdline.Get<bool>("bool"), true);

  // set the value of the long option
  argv = {"ascpp", "--bool=true"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("bool"), true);
  argv = {"ascpp", "--bool=1"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("bool"), true);
  argv = {"ascpp", "--bool=false"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("bool"), false);
  argv = {"ascpp", "--bool=0"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<bool>("bool"), false);
}

TEST(TestCmdline, AddIntOption) {
  ascpp::Cmdline cmdline{&app_info};
  cmdline.AddOption<int>('i', "short_i", "");
  cmdline.AddOption<int>("int", "");
  ascpp::Cmdline cmdline_default{&app_info};
  cmdline_default.AddOptionWithDefault<int>('i', "short_i", "", 11);
  cmdline_default.AddOptionWithDefault<int>("int", "", 11);
  ascpp::Cmdline cmdline_implicit{&app_info};
  cmdline_implicit.AddOptionWithImplicit<int>('i', "short_i", "", 11);
  cmdline_implicit.AddOptionWithImplicit<int>("int", "", 11);
  ascpp::Cmdline cmdline_both{&app_info};
  cmdline_both.AddOptionWithBoth<int>('i', "short_i", "", 11, 11);
  cmdline_both.AddOptionWithBoth<int>("int", "", 11, 11);
  std::vector<const char*> argv{};

  // without default_value
  argv = {"ascpp"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.Get<int>("i"));
  EXPECT_ANY_THROW(cmdline.Get<int>("short_i"));
  EXPECT_ANY_THROW(cmdline.Get<int>("int"));

  // with default_value 11
  argv = {"ascpp"};
  cmdline_default.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.Get<int>("i"), 11);
  EXPECT_EQ(cmdline_default.Get<int>("short_i"), 11);
  EXPECT_EQ(cmdline_default.Get<int>("int"), 11);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<int>("i"), 11);
  EXPECT_EQ(cmdline_both.Get<int>("short_i"), 11);
  EXPECT_EQ(cmdline_both.Get<int>("int"), 11);

  // without implicit_value
  argv = {"ascpp", "-i"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "--int"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));

  // with implicit_value 11
  argv = {"ascpp", "-i"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<int>("i"), 11);
  EXPECT_EQ(cmdline_implicit.Get<int>("short_i"), 11);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<int>("i"), 11);
  EXPECT_EQ(cmdline_both.Get<int>("short_i"), 11);
  argv = {"ascpp", "--int"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<int>("int"), 11);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<int>("int"), 11);

  // set the value of short option without implicit value
  argv = {"ascpp", "-i22"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<int>("i"), 22);
  EXPECT_EQ(cmdline.Get<int>("short_i"), 22);
  argv = {"ascpp", "-i", "22"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<int>("i"), 22);
  EXPECT_EQ(cmdline.Get<int>("short_i"), 22);

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-i22"};
  EXPECT_ANY_THROW(cmdline_implicit.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "-i", "22"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<int>("i"), 11);
  EXPECT_EQ(cmdline_implicit.Get<int>("short_i"), 11);

  // set the argument of the long option
  argv = {"ascpp", "--int=22"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<int>("int"), 22);

  argv = {"ascpp", "--int=nan"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
}

TEST(TestCmdline, AddFloatOption) {
  ascpp::Cmdline cmdline{&app_info};
  cmdline.AddOption<float>('f', "short_f", "");
  cmdline.AddOption<float>("float", "");
  ascpp::Cmdline cmdline_default{&app_info};
  cmdline_default.AddOptionWithDefault<float>('f', "short_f", "", 1.1);
  cmdline_default.AddOptionWithDefault<float>("float", "", 1.1);
  ascpp::Cmdline cmdline_implicit{&app_info};
  cmdline_implicit.AddOptionWithImplicit<float>('f', "short_f", "", 1.1);
  cmdline_implicit.AddOptionWithImplicit<float>("float", "", 1.1);
  ascpp::Cmdline cmdline_both{&app_info};
  cmdline_both.AddOptionWithBoth<float>('f', "short_f", "", 1.1, 1.1);
  cmdline_both.AddOptionWithBoth<float>("float", "", 1.1, 1.1);
  std::vector<const char*> argv{};

  // without default_value
  argv = {"ascpp"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.Get<float>("f"));
  EXPECT_ANY_THROW(cmdline.Get<float>("short_f"));
  EXPECT_ANY_THROW(cmdline.Get<float>("float"));

  // with default_value 1.1
  argv = {"ascpp"};
  cmdline_default.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_default.Get<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_default.Get<float>("short_f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_default.Get<float>("float"), 1.1);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_both.Get<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_both.Get<float>("short_f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_both.Get<float>("float"), 1.1);

  // without implicit_value
  argv = {"ascpp", "-f"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "--float"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));

  // with implicit_value 1.1
  argv = {"ascpp", "-f"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_implicit.Get<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_implicit.Get<float>("short_f"), 1.1);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_both.Get<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_both.Get<float>("short_f"), 1.1);
  argv = {"ascpp", "--float"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_implicit.Get<float>("float"), 1.1);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_both.Get<float>("float"), 1.1);

  // set the value of short option without implicit value
  argv = {"ascpp", "-f2.2"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline.Get<float>("f"), 2.2);
  EXPECT_FLOAT_EQ(cmdline.Get<float>("short_f"), 2.2);
  argv = {"ascpp", "-f", "2.2"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline.Get<float>("f"), 2.2);
  EXPECT_FLOAT_EQ(cmdline.Get<float>("short_f"), 2.2);

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-f2.2"};
  EXPECT_ANY_THROW(cmdline_implicit.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "-f", "2.2"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline_implicit.Get<float>("f"), 1.1);
  EXPECT_FLOAT_EQ(cmdline_implicit.Get<float>("short_f"), 1.1);

  // set the argument of the long option
  argv = {"ascpp", "--float=2.2"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_FLOAT_EQ(cmdline.Get<float>("float"), 2.2);
}

TEST(TestCmdline, AddStringOption) {
  ascpp::Cmdline cmdline{&app_info};
  cmdline.AddOption<std::string>('s', "short_s", "");
  cmdline.AddOption<std::string>("string", "");
  ascpp::Cmdline cmdline_default{&app_info};
  cmdline_default.AddOptionWithDefault<std::string>('s', "short_s", "", "hello");
  cmdline_default.AddOptionWithDefault<std::string>("string", "", "hello");
  ascpp::Cmdline cmdline_implicit{&app_info};
  cmdline_implicit.AddOptionWithImplicit<std::string>('s', "short_s", "", "hello");
  cmdline_implicit.AddOptionWithImplicit<std::string>("string", "", "hello");
  ascpp::Cmdline cmdline_both{&app_info};
  cmdline_both.AddOptionWithBoth<std::string>('s', "short_s", "", "hello", "hello");
  cmdline_both.AddOptionWithBoth<std::string>("string", "", "hello", "hello");
  std::vector<const char*> argv{};

  // without default_value
  argv = {"ascpp"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.Get<std::string>("s"));
  EXPECT_ANY_THROW(cmdline.Get<std::string>("short_s"));
  EXPECT_ANY_THROW(cmdline.Get<std::string>("string"));

  // with default_value "hello"
  argv = {"ascpp"};
  cmdline_default.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_default.Get<std::string>("short_s"), "hello");
  EXPECT_EQ(cmdline_default.Get<std::string>("string"), "hello");
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_both.Get<std::string>("short_s"), "hello");
  EXPECT_EQ(cmdline_both.Get<std::string>("string"), "hello");

  // without implicit_value
  argv = {"ascpp", "-s"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "--string"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));

  // with implicit_value "hello"
  argv = {"ascpp", "-s"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_implicit.Get<std::string>("short_s"), "hello");
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_both.Get<std::string>("short_s"), "hello");
  argv = {"ascpp", "--string"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<std::string>("string"), "hello");
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<std::string>("string"), "hello");

  // set the value of short option without implicit value
  argv = {"ascpp", "-sworld"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "world");
  EXPECT_EQ(cmdline.Get<std::string>("short_s"), "world");
  argv = {"ascpp", "-s", "world"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("s"), "world");
  EXPECT_EQ(cmdline.Get<std::string>("short_s"), "world");

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-sworld"};
  EXPECT_ANY_THROW(cmdline_implicit.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "-s", "world"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<std::string>("s"), "hello");
  EXPECT_EQ(cmdline_implicit.Get<std::string>("short_s"), "hello");

  // set the argument of the long option
  argv = {"ascpp", "--string=world"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::string>("string"), "world");
}

TEST(TestCmdline, AddVectorOption) {
  ascpp::Cmdline cmdline{&app_info};
  cmdline.AddOption<std::vector<std::string>>('V', "short_v", "");
  cmdline.AddOption<std::vector<std::string>>("vector", "");
  ascpp::Cmdline cmdline_default{&app_info};
  cmdline_default.AddOptionWithDefault<std::vector<std::string>>('V', "short_v", "",
                                                                 {"a", "b", "c"});
  cmdline_default.AddOptionWithDefault<std::vector<std::string>>("vector", "", {"a", "b", "c"});
  ascpp::Cmdline cmdline_implicit{&app_info};
  cmdline_implicit.AddOptionWithImplicit<std::vector<std::string>>('V', "short_v", "",
                                                                   {"a", "b", "c"});
  cmdline_implicit.AddOptionWithImplicit<std::vector<std::string>>("vector", "", {"a", "b", "c"});
  ascpp::Cmdline cmdline_both{&app_info};
  cmdline_both.AddOptionWithBoth<std::vector<std::string>>('V', "short_v", "", {"a", "b", "c"},
                                                           {"a", "b", "c"});
  cmdline_both.AddOptionWithBoth<std::vector<std::string>>("vector", "", {"a", "b", "c"},
                                                           {"a", "b", "c"});
  std::vector<const char*> argv{};
  std::vector<std::string> values{"a", "b", "c"};

  // without default_value
  argv = {"ascpp"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_ANY_THROW(cmdline.Get<std::vector<std::string>>("V"));
  EXPECT_ANY_THROW(cmdline.Get<std::vector<std::string>>("short_v"));
  EXPECT_ANY_THROW(cmdline.Get<std::vector<std::string>>("vector"));

  // with default_value {"a","b","c"}
  argv = {"ascpp"};
  cmdline_default.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_default.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_default.Get<std::vector<std::string>>("short_v"), values);
  EXPECT_EQ(cmdline_default.Get<std::vector<std::string>>("vector"), values);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_both.Get<std::vector<std::string>>("short_v"), values);
  EXPECT_EQ(cmdline_both.Get<std::vector<std::string>>("vector"), values);

  // without implicit_value
  argv = {"ascpp", "-V"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "--vector"};
  EXPECT_ANY_THROW(cmdline.Parse(argv.size(), argv.data()));

  // with implicit_value {"a","b","c"}
  argv = {"ascpp", "-V"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_implicit.Get<std::vector<std::string>>("short_v"), values);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_both.Get<std::vector<std::string>>("short_v"), values);
  argv = {"ascpp", "--vector"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<std::vector<std::string>>("vector"), values);
  cmdline_both.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_both.Get<std::vector<std::string>>("vector"), values);

  // set the value of short option without implicit value
  argv = {"ascpp", "-Va,b,c"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("short_v"), values);
  argv = {"ascpp", "-V", "a,b,c"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("short_v"), values);

  // unable to set the value of short option that has implicit_value
  argv = {"ascpp", "-Vx,y,z"};
  EXPECT_ANY_THROW(cmdline_implicit.Parse(argv.size(), argv.data()));
  argv = {"ascpp", "-V", "x,y,z"};
  cmdline_implicit.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline_implicit.Get<std::vector<std::string>>("V"), values);
  EXPECT_EQ(cmdline_implicit.Get<std::vector<std::string>>("short_v"), values);

  // set the argument of the long option
  argv = {"ascpp", "--vector=a,b,c"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("vector"), values);
  argv = {"ascpp", "--vector=a,b,c,"};
  cmdline.Parse(argv.size(), argv.data());
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("vector"), values);
  argv = {"ascpp", "--vector=a,b,c,,"};
  cmdline.Parse(argv.size(), argv.data());
  values.emplace_back("");
  EXPECT_EQ(cmdline.Get<std::vector<std::string>>("vector"), values);
}

}  // namespace ascpp
