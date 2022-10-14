#ifndef ASCPP_UTILS_ERROR_HPP_
#define ASCPP_UTILS_ERROR_HPP_

#include <algorithm>
#include <string>
#include <system_error>

namespace ascpp {

enum AscppErrc { kTestErrc = 1 };

class AscppCategory : public std::error_category {
 public:
  static const AscppCategory& GetInstance() {
    static const AscppCategory kCat{};
    return kCat;
  }

 private:
  AscppCategory() = default;
  ~AscppCategory() override = default;

 public:
  const char* name() const noexcept override { return "ascpp"; }

  std::string message(int condition) const override { return "fuck" + std::to_string(condition); }
};

inline std::error_code make_error_code(AscppErrc ec) {
  return {ec, AscppCategory::GetInstance()};
}

}  // namespace ascpp

#endif  // !ASCPP_UTILS_ERROR_HPP_
