#include "utils/error.hpp"

#include <compare>

#include "gtest/gtest.h"

// NOLINTBEGIN(modernize-use-trailing-return-type)

class myerror : public std::error_category {
 public:
  enum errc { NO_ERR, BAD_ERR };

  auto name() const noexcept -> const char* override { return "myerror"; }

  auto message(int ec) const -> std::string override {
    switch (static_cast<errc>(ec)) {
      case NO_ERR:
        return "no error";
      case BAD_ERR:
        return "bad error";
      default:
        return "unkown error";
    }
  }
};

MAKE_ERROR_CODE(myerror);
ERROR_CODE_ENUM(myerror);

struct debug {
  int val = 0;
  int val2 = 0;

  debug(int i) : val(i) {}

  auto operator<=>(const debug&) const -> std::strong_ordering = default;
};

auto get_void_result() -> ascpp::result<void> {
  return {};
}

auto get_value_result() -> ascpp::result<debug> {
  return debug{1};
}

auto get_error_result() -> ascpp::result<debug> {
  return make_error_code(myerror::BAD_ERR);
}

TEST(TestError, ResultUsage) {
  auto res = get_value_result();
  EXPECT_TRUE(res);
  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value(), debug{1});
  EXPECT_EQ(res->val, 1);

  res = get_error_result();
  EXPECT_FALSE(res);
  EXPECT_EQ(res.has_value(), false);
  EXPECT_ANY_THROW(res.value());
  // res->val will case abort
}

TEST(TestError, ResultConstructorAndAssignment) {
  auto r1 = ascpp::result<debug>(debug{1});
  EXPECT_EQ(r1->val, 1);
  r1 = debug{2};
  EXPECT_EQ(r1->val, 2);

  // ascpp::result<debug>({1, 1}) no match constructor

  auto r3 = ascpp::result<debug>(1);
  EXPECT_EQ(r3->val, 1);
  r3 = 2;
  EXPECT_EQ(r3->val, 2);

  auto r4 = ascpp::result<debug>(ascpp::result<int>(1));
  EXPECT_EQ(r4->val, 1);
  r4 = ascpp::result<int>(2);
  EXPECT_EQ(r4->val, 2);

  // ascpp::result<debug>(ascpp::result<void>()) no match constructor
  auto r5 = ascpp::result<void>(ascpp::result<debug>(1));
  EXPECT_EQ(r5.has_value(), true);
  r5 = ascpp::result<debug>(2);
  EXPECT_EQ(r5.has_value(), true);

  auto r6 = ascpp::result<debug>(make_error_code(myerror::BAD_ERR));
  EXPECT_EQ(r6.has_value(), false);
  EXPECT_EQ(r6.error(), make_error_code(myerror::BAD_ERR));
  r6 = make_error_code(myerror::NO_ERR);
  EXPECT_EQ(r6.error(), make_error_code(myerror::NO_ERR));

  r5 = r6;
  EXPECT_EQ(r5.has_value(), false);
  EXPECT_EQ(r5.error(), r6.error());
}

TEST(TestError, ResultComparsion) {
  EXPECT_EQ(ascpp::result<debug>(1), debug{1});
  EXPECT_EQ(ascpp::result<debug>(1), 1);
  EXPECT_EQ(debug{1}, ascpp::result<debug>(1));
  EXPECT_EQ(1, ascpp::result<debug>(1));

  EXPECT_EQ(ascpp::result<debug>(1), ascpp::result<debug>(1));
  EXPECT_EQ(ascpp::result<debug>(1), ascpp::result<int>(1));
  EXPECT_EQ(ascpp::result<int>(1), ascpp::result<debug>(1));

  EXPECT_NE(ascpp::result<debug>(make_error_code(myerror::BAD_ERR)), ascpp::result<debug>(1));
  EXPECT_NE(ascpp::result<debug>(1), ascpp::result<debug>(make_error_code(myerror::BAD_ERR)));
  EXPECT_EQ(ascpp::result<debug>(make_error_code(myerror::BAD_ERR)),
            ascpp::result<debug>(make_error_code(myerror::BAD_ERR)));
}

#include <type_traits>
#if defined(__GNUC__) || defined(__clang__)
auto try_unwrap() -> ascpp::result<debug> {
  TRY(get_void_result());
  auto val = TRY(get_value_result());
  EXPECT_EQ(val.val, 1);
  auto err = TRY(get_error_result());
  return val;
}

TEST(TestError, TryUnwrap) {
  auto err = try_unwrap();
  EXPECT_EQ(err, get_error_result());
}
#endif

// NOLINTEND(modernize-use-trailing-return-type)
