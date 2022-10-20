#ifndef ASCPP_UTILS_RESULT_HPP_
#define ASCPP_UTILS_RESULT_HPP_

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

namespace ascpp {

template <typename T>
class Result {
 public:
  Result() : var_{} {}

  template <typename U>
    requires(std::is_convertible_v<U, T>)
  Result(U&& val) : var_{std::forward<U>(val)} {}

  Result(std::error_code err) : var_{err} {}

  template <typename U>
    requires(std::is_convertible_v<U, T>)
  Result(Result<U> var) {
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

  auto Unwrap() const& -> const T& {
    if (var_.index() != 0) {
      throw std::system_error(UnwrapErr());
    }
    return std::get<T>(var_);
  }

  auto Unwrap() & -> T& { return const_cast<T&>(static_cast<const Result<T>&>(*this).Unwrap()); }

  auto Unwrap() && -> T {
    return std::move(const_cast<T&>(static_cast<const Result<T>&>(*this).Unwrap()));
  }

  template <typename U>
  auto UnwrapOr(U&& default_value) const& -> T {
    return var_.index() != 0 ? std::forward<U>(default_value) : std::get<T>(var_);
  }

  template <typename U>
  auto UnwrapOr(U&& default_value) && -> T {
    return var_.index() != 0 ? std::forward<U>(default_value) : std::move(std::get<T>(var_));
  }

  template <typename U>
  auto UnwrapOrAssign(U&& default_value) & -> T& {
    if (var_.index() != 0) {
      var_ = std::forward<U>(default_value);
    }
    return std::get<T>(var_);
  }

  template <typename U>
  auto UnwrapOrAssign(U&& default_value) && -> T {
    return std::move(static_cast<Result<T>&>(*this).UnwrapOrAssign(std::forward<T>(default_value)));
  }

  auto UnwrapErr() const -> std::error_code { return std::get<std::error_code>(var_); }

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
