#ifndef ASCPP_UTILS_ERROR_HPP_
#define ASCPP_UTILS_ERROR_HPP_

#include <system_error>

namespace ascpp {

template <typename T>
class Error : public std::error_category {
 public:
  static auto GetInstance() -> Error& {
    static T ec{};
    return ec;
  }

 protected:
  Error() = default;
  ~Error() override = default;
};

class SystemError : public Error<SystemError> {
 public:
  enum ErrorCode { kNoError, kGetEnvError, kGetEnvWithEmptyVale, kSetEnvError };

  SystemError() = default;
  ~SystemError() override = default;

  auto name() const noexcept -> const char* override { return "ascpp:system"; }

  auto message(int ec) const -> std::string override {
    switch (ec) {
      case kGetEnvError:
        return "get env error";
      case kGetEnvWithEmptyVale:
        return "env value is empty";
      case kSetEnvError:
        return "set env error";
      default:
        return "unkown error";
    }
  }
};

inline auto make_error_code(SystemError::ErrorCode ec) -> std::error_code {
  return {ec, SystemError::GetInstance()};
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_ERROR_HPP_
