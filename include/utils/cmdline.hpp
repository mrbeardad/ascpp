#pragma once

#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <ostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "cxxopts.hpp"
#include "fmt/ranges.h"

#include "app/info.hpp"
#include "nlohmann/detail/meta/type_traits.hpp"
#include "utils/misc.hpp"

namespace ascpp {

template <typename T>
concept SimpleArgument
    = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_convertible_v<T, std::string>;

template <typename T>
concept Argument = SimpleArgument<T>
                   || (nlohmann::detail::is_specialization_of<std::vector, T>()
                       && SimpleArgument<typename T::value_type>);

template <typename T>
struct OptionWithDefault : public T {};

template <typename T,
          std::optional<T> DEFAULT_VALUE = std::optional<T>{},
          std::optional<T> IMPLICIT_VALUE = std::optional<T>{}>
class Option {
 public:
  template <Argument T>
  Option(std::string long_opt,
         std::string opt_desc,
         const T& default_value,
         const T& implicit_value,
         std::string opt_group = "");

  template <Argument T>
  Option(char shrot_opt,
         std::string long_opt,
         std::string opt_desc,
         T* opt_arg,
         std::string opt_group = "");

  Option(Option&&) = default;
  Option(const Option&) = default;
  auto operator=(Option&&) -> Option& = default;
  auto operator=(const Option&) -> Option& = default;
  ~Option() = default;

 private:
  std::string short_option_;
  std::string long_option_;
  std::string option_desc_;
  std::string option_group_;
  std::optional<std::string> default_value_;
  std::optional<std::string> implicit_value_;
};

class Cmdline {
 public:
  explicit Cmdline(const AppInfo* app_info)
      : app_info_{app_info}, options_{app_info->app_name, app_info->app_desc} {
    options_.add_options()("h,help", "display usage information");
    options_.add_options()("v,version", "output version number");
  }

  Cmdline(Cmdline&&) = default;
  Cmdline(const Cmdline&) = default;
  auto operator=(Cmdline&&) -> Cmdline& = default;
  auto operator=(const Cmdline&) -> Cmdline& = default;
  ~Cmdline() = default;

  template <typename T>
  auto AddOption(const std::string& long_option, const std::string& description) -> Cmdline& {
    options_.add_options()(long_option, description, cxxopts::value<T>());
  }

  template <typename T>
  auto AddOption(char short_option, const std::string& long_option, const std::string& description)
      -> Cmdline& {
    options_.add_options()(short_option + ","s + long_option, description, cxxopts::value<T>());
  }

  template <typename T>
  auto AddOptionWithDefault(const std::string& long_option,
                            const std::string& description,
                            T&& default_value) -> Cmdline& {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()->default_value(fmt::to_string(default_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithDefault(const std::string& long_option,
                            const std::string& description,
                            T&& default_value) -> Cmdline& {
    options_.add_options()(
        long_option, description,
        cxxopts::value<T>()->default_value(fmt::to_string(fmt::join(default_value, ","))));
  }

  template <typename T>
  auto AddOptionWithDefault(char short_option,
                            const std::string& long_option,
                            const std::string& description,
                            T&& default_value) -> Cmdline& {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()->default_value(fmt::to_string(default_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithDefault(char short_option,
                            const std::string& long_option,
                            const std::string& description,
                            T&& default_value) -> Cmdline& {
    options_.add_options()(
        short_option + ","s + long_option, description,
        cxxopts::value<T>()->default_value(fmt::to_string(fmt::join(default_value, ","))));
  }

  template <typename T>
  auto AddOptionWithImplicit(const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) -> Cmdline& {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()->implicit_value(fmt::to_string(implicit_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithImplicit(const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) -> Cmdline& {
    options_.add_options()(
        long_option, description,
        cxxopts::value<T>()->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  template <typename T>
  auto AddOptionWithImplicit(char short_option,
                             const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) -> Cmdline& {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()->implicit_value(fmt::to_string(implicit_value)));
  }

  template <std::ranges::range T>
    requires(!std::is_convertible_v<T, std::string>)
  auto AddOptionWithImplicit(char short_option,
                             const std::string& long_option,
                             const std::string& description,
                             T&& implicit_value) -> Cmdline& {
    options_.add_options()(
        short_option + ","s + long_option, description,
        cxxopts::value<T>()->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  template <typename T>
  auto AddOptionWithBoth(const std::string& long_option,
                         const std::string& description,
                         T&& default_value,
                         T&& implicit_value) -> Cmdline& {
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
                         T&& implicit_value) -> Cmdline& {
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
                         T&& implicit_value) -> Cmdline& {
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
                         T&& implicit_value) -> Cmdline& {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()
                               ->default_value(fmt::to_string(fmt::join(default_value, ",")))
                               ->implicit_value(fmt::to_string(fmt::join(implicit_value, ","))));
  }

  auto Parse(int argc, const char** argv) -> void {
    result_ = options_.parse(argc, argv);
    if (result_.count("help")) {
      std::cout << options_.help() << std::endl;
      std::exit(0);
    }
    if (result_.count("version")) {
      std::cout << app_info_->app_version << std::endl;
      std::exit(0);
    }
  }

  template <typename T>
  auto Get(const std::string& option) const& -> const T& {
    return result_[option].as<T>();
  }

  auto GetUnmatched() const& -> const std::vector<std::string>& { return result_.unmatched(); }

 private:
  const AppInfo* app_info_;
  cxxopts::Options options_;
  cxxopts::ParseResult result_;
};

}  // namespace ascpp
