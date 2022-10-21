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

}  // namespace ascpp

#endif  // !ASCPP_UTILS_ERROR_HPP_
