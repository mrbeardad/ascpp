#include <iostream>
#include <ostream>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "nlohmann/json.hpp"

#include "utils/cmdline.hpp"
#include "utils/error.hpp"

void cxxopts_ex(int argc, const char** argv) {
  cxxopts::Options options{"ascpp", "async/awesome cpp framework"};
  // options.allow_unrecognised_options();
  options.add_options()("n,none", "none")("o,optional", "optional",
                                          cxxopts::value<int>()->implicit_value("1"))(
      "m,must", "must", cxxopts::value<std::string>());
  auto result = options.parse(argc, argv);
  // std::cout << result["n"].as<bool>() << std::endl;
  // std::cout << result["o"].as<int>() << std::endl;
  // std::cout << result["m"].as<std::string>() << std::endl;
  // for (const auto& e : result.unmatched()) {
  //   std::cout << e << std::endl;
  // }
  // for (auto i = result.begin(); i != result.end(); ++i) {
  //   std::cout << i->key() << ':' << i->value() << std::endl;
  // }
}

void cmdline_ex(int argc, const char** argv) {
  ascpp::Cmdline cmdline{"ascpp", "async and awesome cpp framework"};
  cmdline.AddOption<int>('i', "int", "number");
  cmdline.AddOptionWithDefault<std::string>('s', "str", "string", "fuck");
  cmdline.Parse(argc, argv);
  std::cout << cmdline.Get<int>("int");
}

void json_ex() {
  nlohmann::json j{};
  j["name"] = "xhc";
  j["id"] = 'x';
  std::cout << j["id"].get<int>() << std::endl;
}

void category() {
  std::cout << std::make_error_code(std::errc::executable_format_error).message() << std::endl;
  std::cout << ascpp::make_error_code(ascpp::AscppErrc::kTestErrc).message() << std::endl;
}

void fmtformat() {
  std::cout << fmt::to_string(fmt::join(std::vector{1, 2}, ",")) << std::endl;
  std::cout << fmt::is_range<std::vector<int>, char>::value << std::endl;
}

int main(int argc, const char** argv) {
  fmtformat();
  return 0;
}
