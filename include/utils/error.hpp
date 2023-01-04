#pragma once

#include <array>
#include <expected>
#include <stdexcept>
#include <system_error>
#include <type_traits>

/**
 * @brief Provide std::error_code make_error_code(error_category_derived::errc);
 */
#define MAKE_ERROR_CODE(error_category_derived)                                   \
  inline auto make_error_code(error_category_derived::errc ec)->std::error_code { \
    static auto err_cat = error_category_derived();                               \
    return {ec, err_cat};                                                         \
  }

/**
 * @brief Provide comparison function between error_category_derived::errc and std::error_code
 */
#define ERROR_CODE_ENUM(error_category_derived)                           \
  namespace std {                                                         \
  template <>                                                             \
  struct is_error_code_enum<error_category_derived::errc> : true_type {}; \
  }  // namespace ascpp

namespace ascpp {

class error : public std::error_category {
 public:
  enum errc {
    no_error,
    get_env_failed,
    get_empty_env,
    set_env_failed,
    codecvt_failed,
    INVALID_ARGUMENT,
    OUT_OF_RANGE
  };

  auto name() const noexcept -> const char* override { return "ascpp"; }

  auto message(int ec) const -> std::string override {
    constexpr auto msg = std::array{
        "no error",       "get env failed",   "env value is empty", "set env failed",
        "codecvt failed", "invalid argument", "out of range",
    };
    if (static_cast<unsigned>(ec) < msg.size()) {
      return msg[ec];
    }
    return "unkown error";
  }
};

MAKE_ERROR_CODE(error);

}  // namespace ascpp

ERROR_CODE_ENUM(ascpp::error);

#if defined(__GNUC__) || defined(__clang__)
/**
 * @brief Try to get the result value if it has value, otherwise return the error to upper.
 *
 */
#define TRY(...)                    \
  ({                                \
    auto _ascpp_res_ = __VA_ARGS__; \
    if (!_ascpp_res_.has_value()) { \
      return _ascpp_res_.error();   \
    }                               \
    std::move(_ascpp_res_).value(); \
  })
#endif

namespace ascpp {

class result_error : public std::runtime_error {
 public:
  explicit result_error(const std::error_code& ec) : std::runtime_error(ec.message()), ec_(ec) {}

  auto code() const -> const std::error_code& { return ec_; }

 private:
  std::error_code ec_;
};

template <typename T, typename Base>
  requires(!std::is_same_v<T, std::error_code>)
class [[nodiscard]] result_impl : public Base {
  template <typename U>
  using result = result_impl<U, std::expected<U, std::error_code>>;

 public:
  using base_type = Base;
  using Base::Base;

  result_impl(const std::error_code& ec) : Base(std::unexpected(ec)) {}

  auto operator=(const std::error_code& ec) -> result_impl& {
    Base::operator=(std::unexpected(ec));
    return *this;
  }

  [[nodiscard]] auto value() & -> T& {
    if (!this->has_value()) {
      throw result_error(this->error());
    }
    return static_cast<Base&>(*this).value();
  }

  [[nodiscard]] auto value() const& -> const T& {
    if (!this->has_value()) {
      throw result_error(this->error());
    }
    return static_cast<const Base&>(*this).value();
  }

  [[nodiscard]] auto value() && -> T&& {
    if (!this->has_value()) {
      throw result_error(this->error());
    }
    return static_cast<Base&&>(*this).value();
  }

  [[nodiscard]] auto value() const&& -> const T&& {
    if (!this->has_value()) {
      throw result_error(this->error());
    }
    return static_cast<const Base&&>(*this).value();
  }

  auto operator->() const noexcept -> const T* = delete;

  template <class T2>
  friend constexpr auto operator==(const result_impl& lhs, const result<T2>& rhs) -> bool {
    return static_cast<const base_type&>(lhs)
           == static_cast<const typename result<T2>::base_type&>(rhs);
  }

  template <class T2>
  friend constexpr auto operator==(const result_impl& x, const T2& val) -> bool {
    return static_cast<const base_type&>(x) == val;
  }

  template <class E2>
  friend constexpr auto operator==(const result_impl& x, const std::unexpected<E2>& e) -> bool {
    return static_cast<const base_type&>(x) == e;
  }

  friend constexpr auto operator==(const result_impl& x, const std::error_code& e) -> bool {
    return static_cast<const base_type&>(x) == std::unexpected(e);
  }

  friend constexpr auto operator==(const std::error_code& e, const result_impl& x) -> bool {
    return static_cast<const base_type&>(x) == std::unexpected(e);
  }
};

template <typename Base>
class [[nodiscard]] result_impl<void, Base> : public Base {
  template <typename U>
  using result = result_impl<U, std::expected<U, std::error_code>>;

 public:
  using base_type = Base;
  using Base::Base;

  result_impl(const std::error_code& ec) : Base(std::unexpected(ec)) {}

  template <typename U>
  explicit result_impl(const result<U>& other) {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(other.error()));
    }
  }

  template <typename U>
  explicit result_impl(result<U>&& other) {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(std::move(other.error())));
    }
  }

  auto operator=(const std::error_code& ec) -> result_impl& {
    Base::operator=(std::unexpected(ec));
    return *this;
  }

  template <typename U>
  auto operator=(const result<U>& other) -> result_impl& {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(other.error()));
    }
    return *this;
  }

  template <typename U>
  auto operator=(result<U>&& other) -> result_impl& {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(std::move(other.error())));
    }
    return *this;
  }

  auto value() const& -> void {
    if (!this->has_value()) {
      throw result_error(this->error());
    }
    return static_cast<const Base&>(*this).value();
  }

  auto value() const&& -> void {
    if (!this->has_value()) {
      throw result_error(this->error());
    }
    return static_cast<const Base&&>(*this).value();
  }

  template <class T2>
  friend constexpr auto operator==(const result_impl& lhs, const result<T2>& rhs) -> bool {
    return static_cast<const base_type&>(lhs)
           == static_cast<const typename result<T2>::base_type&>(rhs);
  }

  template <class E2>
  friend constexpr auto operator==(const result_impl& x, const std::unexpected<E2>& e) -> bool {
    return static_cast<const base_type&>(x) == e;
  }

  friend constexpr auto operator==(const result_impl& x, const std::error_code& e) -> bool {
    return static_cast<const base_type&>(x) == std::unexpected(e);
  }

  friend constexpr auto operator==(const std::error_code& e, const result_impl& x) -> bool {
    return static_cast<const base_type&>(x) == std::unexpected(e);
  }
};

/**
 * @brief Wrap the result value in roder to force user to handle unexpected error.
 *
 * @tparam T result value type
 */
template <typename T>
using result = result_impl<T, std::expected<T, std::error_code>>;

}  // namespace ascpp
