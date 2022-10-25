#ifndef ASCPP_UTILS_ERROR_HPP_
#define ASCPP_UTILS_ERROR_HPP_

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

namespace ascpp {

template <typename T>
class Error : public std::error_category {
 public:
  static auto make_error_code(int ec) -> std::error_code {
    static auto err_cat = T{};
    return {ec, err_cat};
  }

 protected:
  Error() = default;
  ~Error() override = default;
};

template <typename T>
class Result;

struct Void {
  static auto GetInstance() -> const Void& {
    static auto ins = Void{};
    return ins;
  }
};

template <>
class [[nodiscard]] Result<void> {
 public:
  Result() = default;

  Result(std::error_code err) : err_{err} {}

  template <typename U>
  Result(const Result<U>& other_res) {
    if (other_res.IsOk()) {
      err_ = std::error_code{};
    } else {
      err_ = other_res.UnwrapErr();
    }
  }

  Result(Result&&) = default;
  Result(const Result&) = default;
  auto operator=(Result&&) -> Result& = default;
  auto operator=(const Result&) -> Result& = default;
  ~Result() = default;

  auto IsOk() const -> bool { return !err_; }

  auto IsErr() const -> bool { return static_cast<bool>(err_); }

  template <typename Callable>
    requires(std::is_same_v<std::invoke_result_t<Callable, Void>,
                            std::invoke_result_t<Callable, std::error_code>>)
  auto Match(Callable&& call) -> std::invoke_result_t<Callable, Void> {
    if (IsOk()) {
      return call(Void{});
    }
    return call(err_);
  }

  auto Unwrap() const -> const Void& {
    if (IsErr()) {
      throw std::system_error(err_);
    }
    return Void::GetInstance();
  }

  auto UnwrapErr() const -> const std::error_code& {
    if (IsOk()) {
      throw std::bad_variant_access{};
    }
    return err_;
  }

 private:
  std::error_code err_;
};

template <typename T>
class [[nodiscard]] Result {
 public:
  Result() = default;

  template <typename U>
    requires(std::is_convertible_v<U, T>)
  Result(U&& val) : var_{std::forward<U>(val)} {}

  Result(std::error_code err) : var_{err} {}

  template <typename U>
    requires(std::is_convertible_v<U, T> && !std::is_same_v<U, T>)
  Result(Result<U> other_res) {
    if (other_res.IsOk()) {
      var_ = std::move(other_res.Unwrap());
    } else {
      var_ = other_res.UnwrapErr();
    }
  }

  Result(Result<void> void_res) {
    if (void_res.IsErr()) {
      var_ = void_res.UnwrapErr();
    }
  }

  Result(Result&&) noexcept = default;
  Result(const Result&) = default;
  auto operator=(Result&&) noexcept -> Result& = default;
  auto operator=(const Result&) -> Result& = default;
  ~Result() = default;

  auto IsOk() const -> bool { return var_.index() == 0; }

  auto IsErr() const -> bool { return var_.index(); }

  template <typename Callable>
  auto Match(Callable&& call) -> decltype(std::visit(call, std::variant<T, std::error_code>{})) {
    return std::visit(call, var_);
  }

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

  auto UnwrapErr() const -> const std::error_code& { return std::get<std::error_code>(var_); }

 private:
  std::variant<T, std::error_code> var_;
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

#endif  // !ASCPP_UTILS_ERROR_HPP_
