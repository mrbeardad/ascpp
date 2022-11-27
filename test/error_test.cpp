#include "utils/error.hpp"

#include <ios>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <variant>

#include "gtest/gtest.h"

auto move_construct_times = 0;
auto move_assign_times = 0;
auto copy_construct_times = 0;
auto copy_assign_times = 0;

auto ResetTimes() -> void {
  move_construct_times = 0;
  move_assign_times = 0;
  copy_construct_times = 0;
  copy_assign_times = 0;
}

struct Debug {
  Debug() {}

  Debug(int i) : value(i) {}

  Debug(Debug&& other) noexcept {
    value = other.value;
    ++move_construct_times;
  }

  Debug(const Debug& other) {
    value = other.value;
    ++copy_construct_times;
  }

  auto operator=(Debug&& other) noexcept -> Debug& {
    value = other.value;
    ++move_assign_times;
    return *this;
  }

  auto operator=(const Debug& other) -> Debug& {
    value = other.value;
    ++copy_assign_times;
    return *this;
  }

  ~Debug() {}

  auto Reset() -> void { value = 0; }

  int value;
};

auto operator==(const Debug& lhs, const Debug& rhs) -> bool {
  return lhs.value == rhs.value;
}

class MyError : public std::error_category {
 public:
  enum Errc { NO_ERR, ERR_1, ERR_2 };

  auto name() const noexcept -> const char* override { return "MyError"; }

  auto message(int ec) const -> std::string override {
    switch (static_cast<Errc>(ec)) {
      case NO_ERR:
        return "no error";
      case ERR_1:
        return "error 1";
      case ERR_2:
        return "error 2";
    }
    return "unkown error";
  }
};

MAKE_ERROR_CODE(MyError);
ERROR_CODE_ENUM(MyError);

TEST(TestError, BasicUsage) {
  auto err = make_error_code(MyError::ERR_1);
  auto res = ascpp::Result<void>(err);
  EXPECT_ANY_THROW(res.Unwrap());
  EXPECT_ANY_THROW(throw std::system_error(err));
}

TEST(TestResult, Constructor) {
  auto res = ascpp::Result<Debug>();
  EXPECT_EQ(res.Unwrap(), 0);

  res.Unwrap().Reset();
  res = Debug(1);
  EXPECT_EQ(res.Unwrap(), 1);

  res.Unwrap().Reset();
  res = 1;
  EXPECT_EQ(res.Unwrap(), 1);

  res.Unwrap().Reset();
  res = 1.5;
  EXPECT_EQ(res.Unwrap(), 1);

  res.Unwrap().Reset();
  res = ascpp::Result<int>(1);
  EXPECT_EQ(res.Unwrap(), 1);

  res.Unwrap().Reset();
  res = ascpp::Result<double>(1.5);
  EXPECT_EQ(res.Unwrap(), 1);

  res.Unwrap().Reset();
  res = ascpp::Result<Debug>(make_error_code(MyError::ERR_1));
  EXPECT_EQ(res.UnwrapErr(), MyError::ERR_1);

  // could not conversion from Result<void> to Result<NonVoid>
  // res.Unwrap().Reset();
  // res = ascpp::Result<void>();
  // EXPECT_EQ(res.Unwrap(), 0);

  auto void_res = ascpp::Result<void>(make_error_code(MyError::ERR_1));
  EXPECT_EQ(void_res.UnwrapErr(), MyError::ERR_1);

  void_res = ascpp::Result<int>();
  EXPECT_EQ(void_res.IsOk(), true);

  res = make_error_code(MyError::ERR_1);
  EXPECT_EQ(res.UnwrapErr(), MyError::ERR_1);
  res = make_error_code(MyError::NO_ERR);
  EXPECT_EQ(res.IsOk(), true);
}

TEST(TestResult, BasicUsage) {
  auto res = ascpp::Result<Debug>();
  EXPECT_EQ(res.IsOk(), true);
  EXPECT_EQ(res.IsErr(), false);
  EXPECT_EQ(res.Unwrap(), 0);
  EXPECT_ANY_THROW(res.UnwrapErr());
  EXPECT_EQ(res.UnwrapOr(Debug(1)), 0);
  EXPECT_EQ(res.UnwrapOr(1), 0);
  EXPECT_EQ(res.UnwrapOrAssign(Debug(1)), 0);
  EXPECT_EQ(res.UnwrapOrAssign(1), 0);
  EXPECT_EQ(res.Match([](auto&& arg) {
    OK(arg) {
      return arg.value;
    }
    ERR(arg) {
      return arg.value();
    }
  }),
            0);

  res = make_error_code(MyError::ERR_1);
  EXPECT_EQ(res.IsErr(), true);
  EXPECT_EQ(res.IsOk(), false);
  EXPECT_EQ(res.UnwrapErr(), MyError::ERR_1);
  EXPECT_ANY_THROW(res.Unwrap());
  EXPECT_EQ(res.UnwrapOr(Debug(1)), 1);
  EXPECT_EQ(res.UnwrapOr(1), 1);
  EXPECT_EQ(res.UnwrapOrAssign(Debug(1)), 1);
  EXPECT_EQ(res.Unwrap(), 1);
  EXPECT_EQ(res.UnwrapOrAssign(2), 1);

  res = make_error_code(MyError::ERR_1);
  EXPECT_EQ(res.Match([](auto&& arg) {
    OK(arg) {
      return arg.value;
    }
    ERR(arg) {
      return arg.value();
    }
  }),
            1);
}

auto GetResult() -> ascpp::Result<Debug> {
  auto re = std::default_random_engine(std::random_device()());
  auto rd = std::bernoulli_distribution();
  if (rd(re)) {
    return -1;
  }
  return make_error_code(MyError::ERR_1);
}

auto UseTryUnwrap() -> ascpp::Result<void> {
  auto res = TRY_UNWRAP(GetResult());
  // do something ...
  return {};
}

TEST(TestResult, TryUnwrap) {
  UseTryUnwrap().Match([](auto&& arg) {
    OK(arg) {
      std::clog << "OK" << std::endl;
    }
    ERR(arg) {
      std::clog << "ERR" << std::endl;
    }
  });
}
