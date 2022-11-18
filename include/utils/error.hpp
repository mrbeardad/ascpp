#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

namespace ascpp {

/**
 * @brief Mixin class that provide static method to make std::error_code. You also need to implement
 * name() and message().
 *
 * @tparam T Derived class
 */
template <typename T>
class Error : public std::error_category {
 public:
  static auto make_error_code(int ec) -> std::error_code {
    static auto err_cat = T{};
    return {ec, err_cat};
  }

 protected:
  Error() = default;

 public:
  ~Error() override = default;
};

struct Void {};

template <typename T>
class [[nodiscard]] Result {
 public:
  using ValueType = std::conditional_t<std::is_same_v<T, void>, Void, T>;

  Result() = default;

  template <typename U>
    requires(std::is_convertible_v<std::decay_t<U>, ValueType>)
  Result(U&& val) : var_{static_cast<ValueType>(std::forward<U>(val))} {}

  template <typename U>
    requires(std::is_convertible_v<U, ValueType>)
  Result(Result<U> other_res) {
    if (other_res.IsOk()) {
      var_ = static_cast<ValueType>(std::move(other_res.Unwrap()));
    } else {
      var_ = other_res.UnwrapErr();
    }
  }

  template <typename U>
    requires(std::is_same_v<ValueType, Void>)
  Result(const Result<U>& other_res) {
    if (other_res.IsOk()) {
      var_ = Void{};
    } else {
      var_ = other_res.UnwrapErr();
    }
  }

  Result(std::error_code err) {
    if (err) {
      var_ = err;
    } else {
      var_ = ValueType{};
    }
  }

  Result(Result&&) noexcept = default;
  Result(const Result&) = default;
  auto operator=(Result&&) noexcept -> Result& = default;
  auto operator=(const Result&) -> Result& = default;
  ~Result() = default;

  auto IsOk() const -> bool { return var_.index() == 0; }

  auto IsErr() const -> bool { return var_.index() != 0; }

  template <typename Callable>
  auto Match(Callable&& call) -> decltype(std::visit(std::forward<Callable>(call),
                                                     std::variant<ValueType, std::error_code>{})) {
    return std::visit(std::forward<Callable>(call), var_);
  }

  auto Unwrap() const -> const ValueType& {
    if (IsErr()) {
      throw std::system_error(UnwrapErr());
    }
    return std::get<ValueType>(var_);
  }

  auto Unwrap() -> ValueType& {
    return const_cast<ValueType&>(static_cast<const Result<T>&>(*this).Unwrap());
  }

  template <typename U>
    requires(std::is_convertible_v<std::decay_t<U>, ValueType>)
  auto UnwrapOr(U&& default_value) const& -> ValueType {
    return IsErr() ? static_cast<ValueType>(std::forward<U>(default_value))
                   : std::get<ValueType>(var_);
  }

  template <typename U>
    requires(std::is_convertible_v<std::decay_t<U>, ValueType>)
  auto UnwrapOr(U&& default_value) && -> ValueType {
    return IsErr() ? static_cast<ValueType>(std::forward<U>(default_value))
                   : std::move(std::get<ValueType>(var_));
  }

  template <typename U>
    requires(std::is_convertible_v<std::decay_t<U>, ValueType>)
  auto UnwrapOrAssign(U&& default_value) -> ValueType& {
    if (IsErr()) {
      var_ = static_cast<ValueType>(std::forward<U>(default_value));
    }
    return std::get<ValueType>(var_);
  }

  auto UnwrapErr() const -> const std::error_code& { return std::get<std::error_code>(var_); }

 private:
  std::variant<ValueType, std::error_code> var_;
};

#define TRY_UNWRAP(...)              \
  ({                                 \
    auto _ascpp_res_ = __VA_ARGS__;  \
    if (_ascpp_res_.IsErr()) {       \
      return _ascpp_res_;            \
    }                                \
    std::move(_ascpp_res_.Unwrap()); \
  })

#define OK(arg)  if constexpr (!std::is_same_v<std::decay_t<decltype(arg)>, std::error_code>)

#define ERR(arg) else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::error_code>)

}  // namespace ascpp
