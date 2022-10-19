#ifndef ASCPP_UTILS_RESULT_HPP_
#define ASCPP_UTILS_RESULT_HPP_

#include <algorithm>
#include <iostream>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

namespace ascpp {

template <typename T>
class Result {
 public:
  Result(T val) : var_{std::move(val)} {}

  Result(std::error_code err) : var_{err} {}

  template <typename OtherT>
    requires(std::is_convertible_v<OtherT, T>)
  Result(Result<OtherT> var) {
    if (var) {
      var_ = std::move(var.Unwrap());
    } else {
      var_ = var.UnwrapErr();
    }
  }

  Result(Result&&) noexcept = default;
  Result(const Result&) = default;
  auto operator=(Result&&) noexcept -> Result& = default;
  auto operator=(const Result&) -> Result& = default;
  ~Result() = default;

  auto Unwrap() & -> T& { return std::get<T>(var_); }

  auto Unwrap() const& -> const T& { return std::get<T>(var_); }

  auto Unwrap() && -> T { return std::move(std::get<T>(var_)); }

  // auto UnwrapOr()

  auto UnwrapErr() -> std::error_code { return std::get<std::error_code>(var_); }

  explicit operator bool() { return var_.index() == 0; }

 private:
  std::variant<T, std::error_code> var_;
};

/**
 * @brief Try to unwrap a result and define a reference named var_name, if result is an error, then
 * return to upper.
 *
 * the 2nd argument 'result' must be a lvalue, otherwise, an extra copy will be made.
 *    auto res = RetResult();
 *    TRY_UNWRAP(val, res);
 *
 */
#define TRY_UNWRAP(var_name, result) \
  if (!(result)) {                   \
    return result;                   \
  }                                  \
  auto&(var_name) = (result).Unwrap();

}  // namespace ascpp

#endif  // !ASCPP_UTILS_RESULT_HPP_
