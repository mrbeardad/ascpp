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
class [[nodiscard]] Result {
 public:
  Result() : var_{} {}

  template <typename U>

  requires(std::is_convertible_v<U, T>) Result(U&& val) : var_{std::forward<U>(val)} {}

  Result(std::error_code err) : var_{err} {}

  template <typename U>

  requires(std::is_convertible_v<U, T>) Result(Result<U> var) {
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

  auto Unwrap() const -> const T& {
    if (IsErr()) {
      throw std::system_error(UnwrapErr());
    }
    return std::get<T>(var_);
  }

  auto Unwrap() -> T& { return const_cast<T&>(static_cast<const Result<T>&>(*this).Unwrap()); }

  template <typename U>
  auto UnwrapOr(U&& default_value) const& -> T {
    return IsErr() ? std::forward<U>(default_value) : std::get<T>(var_);
  }

  template <typename U>
  auto UnwrapOr(U&& default_value) && -> T {
    return IsErr() ? std::forward<U>(default_value) : std::move(std::get<T>(var_));
  }

  template <typename U>
  auto UnwrapOrAssign(U&& default_value) & -> T& {
    if (IsErr()) {
      var_ = std::forward<U>(default_value);
    }
    return std::get<T>(var_);
  }

  template <typename U>
  auto UnwrapOrAssign(U&& default_value) && -> T {
    return std::move(static_cast<Result<T>&>(*this).UnwrapOrAssign(std::forward<T>(default_value)));
  }

  auto UnwrapErr() const -> std::error_code { return std::get<std::error_code>(var_); }

  auto IsOk() const -> bool { return var_.index() == 0; }

  auto IsErr() const -> bool { return var_.index(); }

 private:
  std::variant<T, std::error_code> var_;
};

template <>
class Result<void> {};

#define TRY_UNWRAP(...)              \
  ({                                 \
    auto _ascpp_res_ = __VA_ARGS__;  \
    if (_ascpp_res_.IsErr()) {       \
      return _ascpp_res_;            \
    }                                \
    std::move(_ascpp_res_.Unwrap()); \
  })

}  // namespace ascpp

#endif  // !ASCPP_UTILS_RESULT_HPP_
