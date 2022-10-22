#include "utils/result.hpp"

#include <iostream>
#include <ostream>
#include <system_error>

#include "gtest/gtest.h"

class Debug {
 public:
  Debug() { std::cout << "default construct" << std::endl; }

  Debug(int i) : id{i} { std::cout << "construct: " << id << std::endl; }

  Debug(Debug&& other) {
    id = other.id;
    std::cout << "move construct: " << id << std::endl;
  }

  Debug(const Debug& other) {
    id = other.id;
    std::cout << "copy construct: " << id << std::endl;
  }

  Debug& operator=(Debug&& other) {
    id = other.id;
    std::cout << "move assingment: " << id << std::endl;
    return *this;
  }

  Debug& operator=(const Debug& other) {
    id = other.id;
    std::cout << "copy assingment: " << id << std::endl;
    return *this;
  }

  ~Debug() { std::cout << "destroy: " << id << std::endl; }

  int id;
};

auto GetResult() -> ascpp::Result<Debug> {
  // ascpp::Result<Debug> d{};
  // d = 1;
  // std::cout << "fuck" << std::endl;
  return 1;
}

auto TryToSolveError() -> ascpp::Result<Debug> {
  auto res = GetResult();
  if (res.IsErr()) {
    res = Debug{500};
  }
  auto d = TRY_UNWRAP(res);
  return d;
}

auto TryToSolveErrorWithDefultValue() -> ascpp::Result<Debug> {
  auto res = GetResult();
  std::cout << "returned 1" << std::endl;
  if (res.IsErr()) {
    res = Debug{2};
  }
  auto d = TRY_UNWRAP(res);
  std::cout << "returned 2" << std::endl;
  return d;
}

auto ReturnErrorToUpper() -> ascpp::Result<Debug> {
  auto res = TRY_UNWRAP(GetResult());
  // auto result = GetResult();
  // if (!result) {
  //   return result;
  // }
  // auto& res = result.Unwrap();
  // std::cout << "fuck" << std::endl;
  return res;
}

TEST(TestResult, CouldCompile) {
  ascpp::Result<int> lr{1};
  EXPECT_EQ(lr.Unwrap(), 1);
  EXPECT_EQ(lr.UnwrapOr(2), 1);
  EXPECT_EQ(lr.UnwrapOrAssign(2), 1);
  const auto& clr{lr};
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

  ascpp::Result<void> r{};
  // auto d = ReturnErrorToUpper();
  // std::cout << d.Unwrap().id << std::endl;
}
