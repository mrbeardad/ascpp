#ifndef ASCPP_UTILS_CMDLINE_HPP_
#define ASCPP_UTILS_CMDLINE_HPP_

#include <ranges>
#include <string>
#include <type_traits>
#include <utility>

#include "cxxopts.hpp"
#include "fmt/ranges.h"

#include "async/app.hpp"
#include "utils/misc.hpp"

namespace ascpp {

class Cmdline {
 public:
  explicit Cmdline(App* app) : options_{app->GetAppName(), app->GetAppDescription()} {}

  Cmdline(Cmdline&&) = default;
  Cmdline(const Cmdline&) = default;
  auto operator=(Cmdline&&) -> Cmdline& = default;
  auto operator=(const Cmdline&) -> Cmdline& = default;
  ~Cmdline() = default;

  template <typename T>
  auto AddOption(const std::string& long_option, const std::string& description) {
    options_.add_options()(long_option, description, cxxopts::value<T>());
  }

  template <typename T>
  auto AddOption(char short_option,
                 const std::string& long_option,
                 const std::string& description) {
    options_.add_options()(short_option + ","s + long_option, description, cxxopts::value<T>());
  }

  template <typename T>
  auto AddOptionWithDefault(const std::string& long_option,
                            const std::string& description,
                            T&& default_value) {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()->default_value(fmt::to_string(default_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithDefault(const std::string& long_option,
                            const std::string& description,
                            T&& default_value) {
    options_.add_options()(
        long_option, description,
        cxxopts::value<T>()->default_value(fmt::to_string(fmt::join(default_value, ","))));
  }

  template <typename T>
  auto AddOptionWithDefault(char short_option,
                            const std::string& long_option,
                            const std::string& description,
                            T&& default_value) {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()->default_value(fmt::to_string(default_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithDefault(char short_option,
                            const std::string& long_option,
                            const std::string& description,
                            T&& default_value) {
    options_.add_options()(
        short_option + ","s + long_option, description,
        cxxopts::value<T>()->default_value(fmt::to_string(fmt::join(default_value, ","))));
  }

  template <typename T>
  auto AddOptionWithImplicit(const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()->implicit_value(fmt::to_string(implicit_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithImplicit(const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) {
    options_.add_options()(
        long_option, description,
        cxxopts::value<T>()->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  template <typename T>
  auto AddOptionWithImplicit(char short_option,
                             const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()->implicit_value(fmt::to_string(implicit_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithImplicit(char short_option,
                             const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) {
    options_.add_options()(
        short_option + ","s + long_option, description,
        cxxopts::value<T>()->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  template <typename T>
  auto AddOptionWithBoth(const std::string& long_option,
                         const std::string& description,
                         T&& default_value,
                         T&& implicit_value) {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()
                               ->default_value(fmt::to_string(default_value))
                               ->implicit_value(fmt::to_string(implicit_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithBoth(const std::string& long_option,
                         const std::string& description,
                         T&& default_value,
                         T&& implicit_value) {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()
                               ->default_value(fmt::to_string(fmt::join(default_value, ",")))
                               ->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  template <typename T>
  auto AddOptionWithBoth(char short_option,
                         const std::string& long_option,
                         const std::string& description,
                         T&& default_value,
                         T&& implicit_value) {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()
                               ->default_value(fmt::to_string(default_value))
                               ->implicit_value(fmt::to_string(implicit_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithBoth(char short_option,
                         const std::string& long_option,
                         const std::string& description,
                         T&& default_value,
                         T&& implicit_value) {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()
                               ->default_value(fmt::to_string(fmt::join(default_value, ",")))
                               ->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  auto Parse(int argc, const char** argv) { result_ = options_.parse(argc, argv); }

  template <typename T>
  auto Get(const std::string& option) const& -> const T& {
    return result_[option].as<T>();
  }

  auto GetUnmatched() const& -> const std::vector<std::string>& { return result_.unmatched(); }

 private:
  cxxopts::Options options_;
  cxxopts::ParseResult result_;
};

}  // namespace ascpp

#endif  // !ASCPP_UTILS_CMDLINE_HPP_
