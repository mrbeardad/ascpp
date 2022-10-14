#ifndef ASCPP_UTILS_CMDLINE_HPP_
#define ASCPP_UTILS_CMDLINE_HPP_

#include <algorithm>
#include <string>
#include "cxxopts.hpp"

namespace ascpp {

using std::string_literals::operator""s;

class Cmdline {
 public:
  Cmdline(std::string app_name, std::string app_desc)
      : options_{std::move(app_name), std::move(app_desc)} {}

  Cmdline(Cmdline&&) = default;
  Cmdline(const Cmdline&) = default;
  Cmdline& operator=(Cmdline&&) = default;
  Cmdline& operator=(const Cmdline&) = default;
  ~Cmdline() = default;

  template <typename T>
  void AddOption(const std::string& long_option, const std::string& description) {
    options_.add_options()(long_option, description, cxxopts::value<T>());
  }

  template <typename T>
  void AddOption(char short_option,
                 const std::string& long_option,
                 const std::string& description) {
    options_.add_options()(short_option + ","s + long_option, description, cxxopts::value<T>());
  }

  template <typename T>
  void AddOptionWithDefault(const std::string& long_option,
                            const std::string& description,
                            const std::string& default_value) {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()->default_value(default_value));
  }

  template <typename T>
  void AddOptionWithDefault(char short_option,
                            const std::string& long_option,
                            const std::string& description,
                            const std::string& default_value) {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()->default_value(default_value));
  }

  template <typename T>
  void AddOptionWithImplicit(const std::string& long_option,
                             const std::string& description,
                             const std::string& implicit_value) {
    options_.add_options()(long_option, description,
                           cxxopts::value<T>()->implicit_value(implicit_value));
  }

  template <typename T>
  void AddOptionWithImplicit(char short_option,
                             const std::string& long_option,
                             const std::string& description,
                             const std::string& implicit_value) {
    options_.add_options()(short_option + ","s + long_option, description,
                           cxxopts::value<T>()->implicit_value(implicit_value));
  }

  template <typename T>
  void AddOptionWithBoth(const std::string& long_option,
                         const std::string& description,
                         const std::string& default_value,
                         const std::string& implicit_value) {
    options_.add_options()(
        long_option, description,
        cxxopts::value<T>()->default_value(default_value)->implicit_value(implicit_value));
  }

  template <typename T>
  void AddOptionWithBoth(char short_option,
                         const std::string& long_option,
                         const std::string& description,
                         const std::string& default_value,
                         const std::string& implicit_value) {
    options_.add_options()(
        short_option + ","s + long_option, description,
        cxxopts::value<T>()->default_value(default_value)->implicit_value(implicit_value));
  }

  void Parse(int argc, const char** argv) { result_ = options_.parse(argc, argv); }

  template <typename T>
  const T& get(const std::string& option) const {
    return result_[option].as<T>();
  }

 private:
  cxxopts::Options options_;
  cxxopts::ParseResult result_;
};

}  // namespace ascpp

#endif  // !ASCPP_UTILS_CMDLINE_HPP_
