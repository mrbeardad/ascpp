#include "utils/error.hpp"

#include <iostream>
#include <ostream>
#include <system_error>
#include <type_traits>
#include <utility>
#include <variant>

#include "gtest/gtest.h"

class Debug {
 public:
  Debug() { std::cout << "default construct" << std::endl; }

  Debug(int i) : id{i} { std::cout << "construct: " << id << std::endl; }

  Debug(Debug&& other) noexcept {
    id = other.id;
    std::cout << "move construct: " << id << std::endl;
  }

  Debug(const Debug& other) {
    id = other.id;
    std::cout << "copy construct: " << id << std::endl;
  }

  auto operator=(Debug&& other) noexcept -> Debug& {
    id = other.id;
    std::cout << "move assingment: " << id << std::endl;
    return *this;
  }

  auto operator=(const Debug& other) -> Debug& {
    id = other.id;
    std::cout << "copy assingment: " << id << std::endl;
    return *this;
  }

  ~Debug() { std::cout << "destroy: " << id << std::endl; }

  int id;
};

auto GetResult() -> ascpp::Result<Debug> {
  return 1;
}

auto GetVoidResult() -> ascpp::Result<void> {
  return {};
}

auto UseMatch() -> ascpp::Result<Debug> {
  auto d = GetResult();
  std::cout << d.Match([](auto&& arg) {
    OK(arg) {
      return 1;
    }
    ERR(arg) {
      return 2;
    }
    return 0;
  });

  auto v = GetVoidResult();
  std::cout << v.Match([](auto&& arg) {
    OK(arg) {
      return "a";
    }
    ERR(arg) {
      return "b";
    }
    return "c";
  });
  return {};
}

auto UseUnwrapOr() -> ascpp::Result<Debug> {
  auto d = GetResult().UnwrapOr(2);
  std::cout << "return" << std::endl;
  return d;
}

auto UseUnwrapAssign() -> ascpp::Result<Debug> {
  auto res = GetResult();
  auto& d = res.UnwrapOrAssign(2);
  std::cout << "return" << std::endl;
  return d;
}

auto UseTryUnwrap() -> ascpp::Result<Debug> {
  auto d = TRY_UNWRAP(GetResult());
  // TRY_UNWRAP(GetVoidResult());
  std::cout << "return" << std::endl;
}

TEST(TestResult, Unwrap) {
  ascpp::Result<int> lr{1};
  EXPECT_EQ(lr.Unwrap(), 1);
  EXPECT_EQ(lr.UnwrapOr(2), 1);
  EXPECT_EQ(lr.UnwrapOrAssign(2), 1);
  const auto& clr = lr;
  EXPECT_EQ(clr.Unwrap(), 1);
  EXPECT_EQ(clr.UnwrapOr(2), 1);
  EXPECT_EQ(ascpp::Result<int>{1}.Unwrap(), 1);
  EXPECT_EQ(ascpp::Result<int>{1}.UnwrapOr(2), 1);
  EXPECT_EQ(ascpp::Result<int>{1}.UnwrapOrAssign(2), 1);

  auto ec = std::make_error_code(std::errc::io_error);
  lr = ec;
  EXPECT_EQ(lr.UnwrapErr(), ec);
  EXPECT_EQ(lr.UnwrapOr(2), 2);
  EXPECT_EQ(lr.UnwrapOrAssign(2), 2);
  EXPECT_EQ(lr.Unwrap(), 2);
  lr = ec;
  EXPECT_ANY_THROW(lr.Unwrap());
  const ascpp::Result<int> cl{ec};
  EXPECT_ANY_THROW(cl.Unwrap());
  EXPECT_ANY_THROW(ascpp::Result<int>{ec}.Unwrap());

  ec = std::error_code{};
  lr = 1;
  lr = ec;
  EXPECT_EQ(lr.Unwrap(), 0);
}

TEST(TestResult, VoidUnwrap) {
  ascpp::Result<void> lr{};
  EXPECT_TRUE(lr.IsOk());
  const auto& clr = lr;
  EXPECT_TRUE(clr.IsOk());
  EXPECT_TRUE(ascpp::Result<void>{}.IsOk());

  auto ec = std::make_error_code(std::errc::io_error);
  lr = ec;
  EXPECT_EQ(lr.UnwrapErr(), ec);
  EXPECT_ANY_THROW(lr.Unwrap());
  const ascpp::Result<void> cl{ec};
  EXPECT_ANY_THROW(cl.Unwrap());
  EXPECT_ANY_THROW(ascpp::Result<void>{ec}.Unwrap());

  ascpp::Result<int> int_res{1};
  ascpp::Result<void> void_res{};

  // ok int <- ok void
  EXPECT_EQ(int_res.Unwrap(), 1);
  EXPECT_EQ(void_res.IsOk(), true);
  // int_res = void_res;
  EXPECT_EQ(int_res.Unwrap(), 0);
  EXPECT_EQ(void_res.IsOk(), true);

  int_res = 1;

  // ok int -> ok void
  EXPECT_EQ(int_res.Unwrap(), 1);
  EXPECT_EQ(void_res.IsOk(), true);
  void_res = int_res;
  EXPECT_EQ(int_res.Unwrap(), 1);
  EXPECT_EQ(void_res.IsOk(), true);

  void_res = ec;

  // ok int <- err void
  EXPECT_EQ(int_res.Unwrap(), 1);
  EXPECT_EQ(void_res.UnwrapErr(), ec);
  // int_res = void_res;
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.UnwrapErr(), ec);

  int_res = 1;

  // ok int -> err void
  EXPECT_EQ(int_res.Unwrap(), 1);
  EXPECT_EQ(void_res.UnwrapErr(), ec);
  void_res = int_res;
  EXPECT_EQ(int_res.Unwrap(), 1);
  EXPECT_EQ(void_res.IsOk(), true);

  int_res = ec;

  // err int <- ok void
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.IsOk(), true);
  // int_res = void_res;
  EXPECT_EQ(int_res.Unwrap(), 0);
  EXPECT_EQ(void_res.IsOk(), true);

  int_res = ec;

  // err int -> ok void
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.IsOk(), true);
  void_res = int_res;
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.UnwrapErr(), ec);

  auto ec2 = std::make_error_code(std::errc::executable_format_error);
  void_res = ec2;

  // err int <- err void
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.UnwrapErr(), ec2);
  // int_res = void_res;
  EXPECT_EQ(int_res.UnwrapErr(), ec2);
  EXPECT_EQ(void_res.UnwrapErr(), ec2);

  int_res = ec;

  // err int -> err void
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.UnwrapErr(), ec2);
  void_res = int_res;
  EXPECT_EQ(int_res.UnwrapErr(), ec);
  EXPECT_EQ(void_res.UnwrapErr(), ec);
}
