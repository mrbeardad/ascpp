#pragma once

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
#include "nlohmann/detail/meta/type_traits.hpp"

#include "app/info.hpp"
#include "utils/misc.hpp"

namespace ascpp {

template <typename T>
concept SingleArg
    = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<T, std::string>;

template <typename T>
concept MultiArg
    = nlohmann::detail::is_specialization_of<std::vector, T>() && SingleArg<typename T::value_type>;

template <typename T>
concept SingleOrMultiArg = SingleArg<T> || MultiArg<T>;

namespace {

inline auto ToString(const std::string& s) -> const std::string& {
  return s;
}

template <SingleArg T>
auto ToString(T t) -> std::string {
  return std::to_string(t);
}

template <MultiArg T>
auto ToString(const T& v) -> std::string {
  return fmt::to_string(fmt::join(v, ","));
}

}  // namespace

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

  template <SingleOrMultiArg T>
  auto AddOption(const std::string& long_opt,
                 const std::string& opt_desc,
                 bool positional = false,
                 const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, "", long_opt, opt_desc, cxxopts::value<T>(), "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOption(char short_opt,
                 const std::string& long_opt,
                 const std::string& opt_desc,
                 bool positional = false,
                 const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, std::string{short_opt}, long_opt, opt_desc, cxxopts::value<T>(),
                        "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOptionWithDefault(const std::string& long_opt,
                            const std::string& opt_desc,
                            T&& default_value,
                            bool positional = false,
                            const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, "", long_opt, opt_desc,
                        cxxopts::value<T>()->default_value(ToString(default_value)), "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOptionWithDefault(char short_opt,
                            const std::string& long_opt,
                            const std::string& opt_desc,
                            T&& default_value,
                            bool positional = false,
                            const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, std::string{short_opt}, long_opt, opt_desc,
                        cxxopts::value<T>()->default_value(ToString(default_value)), "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOptionWithImplicit(const std::string& long_opt,
                             const std::string& opt_desc,
                             T&& implicit_value,
                             bool positional = false,
                             const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, "", long_opt, opt_desc,
                        cxxopts::value<T>()->implicit_value(ToString(implicit_value)), "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOptionWithImplicit(char short_opt,
                             const std::string& long_opt,
                             const std::string& opt_desc,
                             T&& implicit_value,
                             bool positional = false,
                             const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, std::string{short_opt}, long_opt, opt_desc,
                        cxxopts::value<T>()->implicit_value(ToString(implicit_value)), "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOptionWithBoth(const std::string& long_opt,
                         const std::string& opt_desc,
                         T&& default_value,
                         T&& implicit_value,
                         bool positional = false,
                         const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, "", long_opt, opt_desc,
                        cxxopts::value<T>()
                            ->default_value(ToString(default_value))
                            ->implicit_value(ToString(implicit_value)),
                        "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  template <SingleOrMultiArg T>
  auto AddOptionWithBoth(char short_opt,
                         const std::string& long_opt,
                         const std::string& opt_desc,
                         T&& default_value,
                         T&& implicit_value,
                         bool positional = false,
                         const std::string& opt_group = "") -> Cmdline& {
    options_.add_option(opt_group, std::string{short_opt}, long_opt, opt_desc,
                        cxxopts::value<T>()
                            ->default_value(ToString(default_value))
                            ->implicit_value(ToString(implicit_value)),
                        "");
    if (positional) {
      positional_options_.emplace_back(long_opt);
    }
    return *this;
  }

  auto Parse(int argc, const char** argv) -> void {
    if (positional_options_.size() > 0) {
      options_.parse_positional(positional_options_);
    }
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
  auto GetArg(const std::string& option) const& -> const T& {
    return result_[option].as<T>();
  }

  auto GetUnmatched() const& -> const std::vector<std::string>& { return result_.unmatched(); }

 private:
  const AppInfo* app_info_;
  cxxopts::Options options_;
  std::vector<std::string> positional_options_;
  cxxopts::ParseResult result_;
};

}  // namespace ascpp
