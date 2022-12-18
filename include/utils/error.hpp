#pragma once

#include <expected>
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
    NO_ERROR,
    GET_ENV_FAILED,
    GET_EMPTY_ENV,
    SET_ENV_FAILED,
    CODECVT_FAILED,
    INVALID_ARGUMENT,
    OUT_OF_RANGE
  };

  auto name() const noexcept -> const char* override { return "ascpp"; }

  auto message(int ec) const -> std::string override {
    switch (ec) {
      case GET_ENV_FAILED:
        return "get env failed";
      case GET_EMPTY_ENV:
        return "env value is empty";
      case SET_ENV_FAILED:
        return "set env failed";
      case CODECVT_FAILED:
        return "codecvt failed";
      case INVALID_ARGUMENT:
        return "invalid argument";
      case OUT_OF_RANGE:
        return "out of range";
      default:
        return "unkown error";
    }
  }
};

MAKE_ERROR_CODE(error);

}  // namespace ascpp

ERROR_CODE_ENUM(ascpp::error);

#if defined(__GNUC__) || defined(__clang__)
/**
 * @brief Try to get the result when it has value or return the error to upper.
 *
 */
#define TRY(...)                    \
  ({                                \
    auto _ascpp_res_ = __VA_ARGS__; \
    if (!_ascpp_res_.has_value()) { \
      return _ascpp_res_.error();   \
    }                               \
    std::move(_ascpp_res_.value()); \
  })
#endif

namespace ascpp {

template <typename T, typename Base>
  requires(!std::is_same_v<T, std::error_code>)
class result_impl : public Base {
  template <typename U>
  using result = result_impl<U, std::expected<U, std::error_code>>;

 public:
  using base_type = Base;
  using Base::Base;

  result_impl(std::error_code ec) : Base(std::unexpected(ec)) {}

  template <typename U>
    requires(std::is_void_v<T>)
  explicit result_impl(const result<U>& other) {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(other.error()));
    }
  }

  template <typename U>
    requires(std::is_void_v<T>)
  explicit result_impl(result<U>&& other) {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(std::move(other.error())));
    }
  }

  auto operator=(std::error_code ec) -> result_impl& {
    Base::operator=(std::unexpected(ec));
    return *this;
  }

  template <typename U>
    requires(std::is_void_v<T>)
  auto operator=(const result<U>& other) -> result_impl& {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(other.error()));
    }
    return *this;
  }

  template <typename U>
    requires(std::is_void_v<T>)
  auto operator=(result<U>&& other) -> result_impl& {
    if (other.has_value()) {
      Base::emplace();
    } else {
      Base::operator=(std::unexpected(std::move(other.error())));
    }
    return *this;
  }

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
};

template <typename T>
using result = result_impl<T, std::expected<T, std::error_code>>;

}  // namespace ascpp
