#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <cctype>
#include <cstddef>
#include <exception>
#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "app/info.hpp"
#include "utils/error.hpp"
#include "utils/misc.hpp"

namespace ascpp {

template <typename T>
concept single_option
    = std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, size_t>
      || std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, std::string>;

template <typename T>
concept multi_option = is_specialization_v<T, std::vector> && single_option<typename T::value_type>;

template <typename T>
concept option_type = single_option<T> || multi_option<T>;

struct option {
  // Use enum instead of std::any::type, so switch statement is available in runtime
  enum type {
    single_bool,
    single_int,
    single_size,
    single_float,
    single_double,
    single_string,
    multiple_bool,
    multiple_int,
    multiple_size,
    multiple_float,
    multiple_double,
    multiple_string,
  };

  template <option_type T>
  static constexpr auto get_type() -> type {
    if constexpr (std::is_same_v<T, bool>) {
      return single_bool;
    } else if constexpr (std::is_same_v<T, int>) {
      return single_int;
    } else if constexpr (std::is_same_v<T, size_t>) {
      return single_size;
    } else if constexpr (std::is_same_v<T, float>) {
      return single_float;
    } else if constexpr (std::is_same_v<T, double>) {
      return single_double;
    } else if constexpr (std::is_same_v<T, std::string>) {
      return single_string;
    } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
      return multiple_bool;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
      return multiple_int;
    } else if constexpr (std::is_same_v<T, std::vector<size_t>>) {
      return multiple_size;
    } else if constexpr (std::is_same_v<T, std::vector<float>>) {
      return multiple_float;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
      return multiple_double;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
      return multiple_string;
    }
  }

  static constexpr auto type_name(type type) -> const char* {
    constexpr auto names
        = std::array{"bool",           "int",           "size_t",         "float",
                     "double",         "string",        "list of bool",   "list of int",
                     "list of size_t", "list of float", "list of double", "list of string"};
    return names[static_cast<size_t>(type)];
  }

  template <single_option T>
  auto check_value(const T& value) const -> bool {
    if (!limits.has_value()) {
      return true;
    }
    if (limits.type() == typeid(std::unordered_set<T>)) {
      return std::any_cast<const std::unordered_set<T>&>(limits).contains(value);
    }
    return std::any_cast<const std::function<bool(const T&)>&>(limits)(value);
  }

  template <multi_option T>
  auto check_value(const T& values) const -> typename T::const_iterator {
    if (!limits.has_value()) {
      return values.end();
    }
    if (limits.type() == typeid(std::unordered_set<typename T::value_type>)) {
      auto& limit_set = std::any_cast<const std::unordered_set<typename T::value_type>&>(limits);
      return std::ranges::find_if(values, [&limit_set](auto e) { return !limit_set.contains(e); });
    }
    auto& limit_fn
        = std::any_cast<const std::function<bool(const typename T::value_type&)>&>(limits);
    return std::ranges::find_if(values, [&limit_fn](auto e) { return !limit_fn(e); });
  }

  type opt_type;
  std::string short_opt;
  std::string long_opt;
  std::string opt_desc;
  std::any transform = {};
  std::any limits = {};  ///< Store the limit value set or limit callable
  std::any default_value = {};
  std::any implicit_value = {};
  std::any result_value = {};
};

template <typename T, typename = void>
struct option_value_type;

template <typename T>
struct option_value_type<T, std::enable_if_t<single_option<T>>> {
  using type = T;
};

template <typename T>
struct option_value_type<T, std::enable_if_t<multi_option<T>>> {
  using type = typename T::value_type;
};

template <option_type T>
class option_adder {
 public:
  using value_type = typename option_value_type<T>::type;

  explicit option_adder(option* opt) : _opt(opt) {}

  auto with_transform(std::function<value_type(std::string_view)> transform) -> option_adder& {
    _opt->transform = std::move(transform);
    return *this;
  }

  auto with_limits(std::unordered_set<value_type> limit_set) -> option_adder& {
    _opt->limits = std::move(limit_set);
    return *this;
  }

  auto with_limits(std::function<bool(const value_type&)> limit_fn) -> option_adder& {
    _opt->limits = std::move(limit_fn);
    return *this;
  }

  auto with_default(T default_value) -> option_adder& {
    if constexpr (single_option<T>) {
      if (!_opt->check_value(default_value)) {
        throw std::logic_error(std::format("the default value is invalid for {} option '{}': {}",
                                           option::type_name(_opt->opt_type), _opt->long_opt,
                                           default_value));
      }
    } else {
      if (auto itr = _opt->check_value(default_value); itr != default_value.end()) {
        throw std::logic_error(
            std::format("the value in default values is invalid for {} option '{}': {}",
                        option::type_name(_opt->opt_type), _opt->long_opt, *itr));
      }
    }
    _opt->default_value = std::move(default_value);
    return *this;
  }

  auto with_implicit(T implicit_value) -> option_adder& {
    if constexpr (single_option<T>) {
      if (!_opt->check_value(implicit_value)) {
        throw std::logic_error(std::format("the implicit value is invalid for {} option '{}' : {}",
                                           option::type_name(_opt->opt_type), _opt->long_opt,
                                           implicit_value));
      }
    } else {
      if (auto itr = _opt->check_value(implicit_value); itr != implicit_value.end()) {
        throw std::logic_error(
            std::format("the value in implicit values is invalid for {} option '{}': {}",
                        option::type_name(_opt->opt_type), _opt->long_opt, *itr));
      }
    }
    _opt->implicit_value = std::move(implicit_value);
    return *this;
  }

 private:
  option* _opt;
};

namespace detail {

template <option_type T>
auto transform_arg(std::string_view) -> T;

template <>
inline auto transform_arg<bool>(std::string_view arg) -> bool {
  auto value = ""s;
  std::ranges::transform(arg, std::back_inserter(value),
                         [](char c) { return std::tolower(static_cast<unsigned char>(c)); });

  if (value == "yes" || value == "on" || value == "true" || value == "1") {
    return true;
  }
  if (value.empty() || value == "no" || value == "off" || value == "false" || value == "0") {
    return false;
  }
  throw std::bad_expected_access<std::error_code>(make_error_code(error::INVALID_ARGUMENT));
}

template <>
inline auto transform_arg<int>(std::string_view arg) -> int {
  if (arg.empty()) {
    return 0;
  }
  return to_number<int>(arg).value();
}

template <>
inline auto transform_arg<size_t>(std::string_view arg) -> size_t {
  if (arg.empty()) {
    return 0;
  }
  return to_number<size_t>(arg).value();
}

template <>
inline auto transform_arg<float>(std::string_view arg) -> float {
  if (arg.empty()) {
    return 0;
  }
  return to_number<float>(arg).value();
}

template <>
inline auto transform_arg<double>(std::string_view arg) -> double {
  if (arg.empty()) {
    return 0;
  }
  return to_number<double>(arg).value();
}

template <>
inline auto transform_arg<std::string>(std::string_view arg) -> std::string {
  return std::string(arg);
}

}  // namespace detail

class cmdline {
 public:
  explicit cmdline(const app_info* app_info) : _app_info(app_info) {}

  template <option_type T>
  auto add_option(std::string long_opt, std::string opt_desc) -> option_adder<T> {
    return _add_option<T>(""s, std::move(long_opt), std::move(opt_desc));
  }

  template <option_type T>
  auto add_option(char short_opt, std::string long_opt, std::string opt_desc) -> option_adder<T> {
    return _add_option<T>(std::string{short_opt}, std::move(long_opt), std::move(opt_desc));
  }

  auto allow_nonoptions(std::string name, bool required) -> void {
    // empty nonopt_name_ means nonoptions are disallowed
    if (name.empty()) {
      throw std::logic_error("empty name for nonoptions is not allowed.");
    }
    _nonopt_name = std::move(name);
    _is_nonopt_required = required;
  }

  auto get_option(const std::string& long_opt) const -> const option& {
    return _options[_search_idx.at(long_opt)];
  }

  auto get_option(char short_opt) const -> const option& {
    return _options[_search_idx.at(std::string{short_opt})];
  }

  auto help_string() const -> std::string {
    auto longopt_str = [](const option& opt) {
      auto ret = ""s;
      if (!opt.implicit_value.has_value()) {
        ret = std::format("--{}=<{}>", opt.long_opt, option::type_name(opt.opt_type));
      } else if (opt.opt_type != option::single_bool) {
        ret = std::format("--{}[=<{}>]", opt.long_opt, option::type_name(opt.opt_type));
      } else {
        ret = std::format("--{}", opt.long_opt);
      }
      return ret;
    };

    auto ret = std::format("{} {}\n{}\n\nUSAGE:\n  {} [OPTIONS] ", _app_info->app_name(),
                           _app_info->app_version(), _app_info->app_intro(), _app_info->app_name());
    if (!_nonopt_name.empty()) {
      ret += "[--] ";
      if (_is_nonopt_required) {
        ret += _nonopt_name;
      } else {
        ret += "[" + _nonopt_name + "]";
      }
    }
    ret += "\n\n";

    auto max_width_of_opt_name_col = static_cast<size_t>(0);
    auto required_opt_name_col = std::vector<std::string>();
    auto required_opt_name_col_width = std::vector<size_t>();
    auto required_opt_desc_col = std::vector<const std::string*>();
    auto optional_opt_name_col = std::vector<std::string>();
    auto optional_opt_name_col_width = std::vector<size_t>();
    auto optional_opt_desc_col = std::vector<const std::string*>();

    for (auto& opt : _options) {
      auto opt_name_col = &required_opt_name_col;
      auto opt_name_col_width = &required_opt_name_col_width;
      auto opt_desc_col = &required_opt_desc_col;
      if (opt.default_value.has_value()) {
        opt_name_col = &optional_opt_name_col;
        opt_name_col_width = &optional_opt_name_col_width;
        opt_desc_col = &optional_opt_desc_col;
      }

      auto opt_name = "  "s;
      if (!opt.short_opt.empty()) {
        opt_name += "-" + opt.short_opt + ", ";
      } else {
        opt_name += "    ";
      }
      opt_name += longopt_str(opt);
      opt_name_col->emplace_back(opt_name);

      auto width = display_width(opt_name);
      if (width) {
        max_width_of_opt_name_col = std::max(max_width_of_opt_name_col, *width);
        opt_name_col_width->emplace_back(*width);
      } else {
        opt_name_col_width->emplace_back(0);
      }

      opt_desc_col->emplace_back(&opt.opt_desc);
    }

    if (!required_opt_name_col.empty()) {
      ret += "REQUIRED OPTIONS:\n";
      for (auto i = 0UZ; i < required_opt_name_col.size(); ++i) {
        ret += required_opt_name_col[i]
               + std::string(required_opt_name_col_width[i] != 0
                                 ? max_width_of_opt_name_col - required_opt_name_col_width[i]
                                 : 1,
                             ' ');
        ret += "\t-- " + *required_opt_desc_col[i] + "\n";
      }
      ret += "\n";
    }
    if (!optional_opt_name_col.empty()) {
      ret += "OPTIONAL OPTIONS:\n";
      for (auto i = 0UZ; i < optional_opt_desc_col.size(); ++i) {
        ret += optional_opt_name_col[i]
               + std::string(max_width_of_opt_name_col - optional_opt_name_col_width[i], ' ');
        ret += "\t-- " + *optional_opt_desc_col[i] + "\n";
      }
    }

    return ret;
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  auto parse_args(int argc, const char* const argv[], bool print_and_exit = false) -> void try {
    for (auto& opt : _options) {
      opt.result_value.reset();
    }
    _nonoptions.clear();

    auto end_parse = false;

    for (auto i = 1; i < argc; ++i) {
      auto this_arg = std::string(argv[i]);
      if (end_parse) {
        _nonoptions.emplace_back(this_arg);
        continue;
      }
      if (this_arg == "--") {
        end_parse = true;
      } else if (this_arg.starts_with("--")) {
        auto es_pos = this_arg.find('=');
        auto opt_name = this_arg.substr(2, es_pos - 2);

        auto opt_idx = _search_idx.find(opt_name);
        if (opt_idx == _search_idx.end()) {
          throw std::runtime_error("no option '" + opt_name + "'");
        }
        if (opt_name.size() < 2) {
          throw std::runtime_error("wrong form for short option '" + this_arg + "'");
        }
        auto& opt = _options[opt_idx->second];

        if (es_pos == std::string::npos) {
          // form: --option [value]
          if (!opt.implicit_value.has_value()) {
            if (i + 1 >= argc) {
              throw std::runtime_error("requires a value for option '" + opt_name + "'");
            }
            _set_value(opt, opt_name, argv[++i]);
          } else {
            opt.result_value = opt.implicit_value;
          }
        } else {
          // form: --option=[value]
          if (es_pos + 1 >= this_arg.size()) {
            _set_value(opt, opt_name, "");
          } else {
            _set_value(opt, opt_name, this_arg.substr(es_pos + 1));
          }
        }
      } else if (this_arg.starts_with("-")) {
        for (auto j = 1UZ; j < this_arg.size(); ++j) {
          auto cur_opt = std::string{this_arg[j]};
          auto opt_idx = _search_idx.find(cur_opt);
          if (opt_idx == _search_idx.end()) {
            throw std::runtime_error("no option '" + cur_opt + "'");
          }
          auto& opt = _options[opt_idx->second];
          if (!opt.implicit_value.has_value()) {
            if (j + 1 >= this_arg.size()) {
              // form: -opt value
              if (i + 1 >= argc) {
                throw std::runtime_error("requires a value for option '" + cur_opt + "'");
              }
              _set_value(opt, cur_opt, argv[++i]);
            } else {
              // form: -optvalue
              _set_value(opt, cur_opt, this_arg.substr(j + 1));
              break;
            }
          } else if (opt.opt_type != option::single_bool) {
            if (j + 1 >= this_arg.size()) {
              // form: -opt
              opt.result_value = opt.implicit_value;
            } else {
              // form: -optvlaue
              _set_value(opt, cur_opt, this_arg.substr(j + 1));
              break;
            }
          } else {
            opt.result_value = opt.implicit_value;
          }
        }
      } else {
        _nonoptions.emplace_back(this_arg);
      }
    }

    if (_nonopt_name.empty() && !_nonoptions.empty()) {
      throw std::runtime_error("nonoption arguments are not allowed");
    }
    if (_is_nonopt_required && _nonoptions.empty()) {
      throw std::runtime_error("required nonoption arguments as " + _nonopt_name);
    }

    for (auto& opt : _options) {
      if (!opt.result_value.has_value()) {
        if (!opt.default_value.has_value()) {
          throw std::runtime_error("requires option '" + opt.long_opt + "'");
        }
        opt.result_value = opt.default_value;
      }
    }
  } catch (const std::exception& excep) {
    if (print_and_exit) {
      std::cerr << excep.what() << std::endl;
      std::cerr << "---\n" << std::endl;
      std::cerr << help_string() << std::endl;
      std::exit(1);
    }
    throw;
  }

  template <option_type T>
  auto get_value(const std::string& opt_name) const -> const T& {
    return std::any_cast<const T&>(_options[_search_idx.at(opt_name)].result_value);
  }

  template <option_type T>
  auto get_value(char opt_name) const -> const T& {
    return get_value<const T>(std::string{opt_name});
  }

  auto get_nonoptions() const -> const std::vector<std::string>& { return _nonoptions; }

 private:
  template <option_type T>
  auto _add_option(std::string short_opt, std::string long_opt, std::string opt_desc)
      -> option_adder<T> {
    if (short_opt.size() == 1) {
      _check_optname(short_opt[0]);
      _search_idx[short_opt] = _options.size();
    } else {
      short_opt.clear();
    }

    _check_optname(long_opt);
    _search_idx[long_opt] = _options.size();
    _options.emplace_back(option{option::get_type<T>(), std::move(short_opt), std::move(long_opt),
                                 std::move(opt_desc)});
    auto opt_adder = option_adder<T>(&_options.back());

    if constexpr (std::is_same_v<T, bool>) {
      opt_adder.with_default(false).with_implicit(true);
    }
    opt_adder.with_transform(&detail::transform_arg<typename option_adder<T>::value_type>);

    return opt_adder;
  }

  auto _check_optname(char opt_name) const -> void {
    auto str_opt = std::string{opt_name};
    if (_search_idx.find(str_opt) != _search_idx.end()) {
      throw std::logic_error("duplicate option '" + str_opt + "'");
    }
    if (!std::isgraph(static_cast<unsigned char>(opt_name))) {
      throw std::logic_error("short option name must be a graphical character: '" + str_opt + "'");
    }
    if (opt_name == '-') {
      throw std::logic_error("short option name could not be '-'");
    }
  }

  auto _check_optname(const std::string& opt_name) const -> void {
    if (_search_idx.find(opt_name) != _search_idx.end()) {
      throw std::logic_error("duplicate option '" + opt_name + "'");
    }
    if (opt_name.size() <= 2) {
      throw std::logic_error("long option name requires at least 2 character: '" + opt_name + "'");
    }
    if (!std::ranges::all_of(opt_name,
                             [](char c) { return std::isgraph(static_cast<unsigned char>(c)); })) {
      throw std::logic_error("long option name must be graphical string: '" + opt_name + "'");
    }
    if (opt_name.starts_with("-")) {
      throw std::logic_error("long option name could not starts with '-'");
    }
    if (opt_name.find('=') != std::string::npos) {
      throw std::logic_error("long option name could not contain '='");
    }
  }

  static auto _set_value(option& opt, const std::string& opt_name, const std::string& opt_value)
      -> void {
    auto parse_value = [&opt, &opt_name]<single_option T>(std::string_view opt_value) {
      if (!opt.transform.has_value()) {
        throw std::runtime_error("no transform function for option '" + opt_name + "'");
      }
      auto value = std::any_cast<std::function<T(std::string_view)>&>(opt.transform)(opt_value);

      if (!opt.check_value(value)) {
        throw std::logic_error(std::format("invalid value for {} option '{}': {}",
                                           option::type_name(opt.opt_type), opt_name, opt_value));
      }
      return value;
    };

    auto try_to_set_value = [&opt, &opt_value, &parse_value]<option_type T>() {
      if constexpr (single_option<T>) {
        opt.result_value = parse_value.operator()<T>(opt_value);
      } else {
        auto vec = std::vector<typename T::value_type>();
        for (auto word : std::views::split(opt_value, ","s)) {
          auto sv = std::string_view(word.begin(), word.end());
          vec.emplace_back(parse_value.operator()<typename T::value_type>(sv));
        }
        opt.result_value = std::move(vec);
      }
    };

    try {
      switch (opt.opt_type) {
        case option::single_bool:
          try_to_set_value.operator()<bool>();
          break;

        case option::single_int:
          try_to_set_value.operator()<int>();
          break;

        case option::single_size:
          try_to_set_value.operator()<size_t>();
          break;

        case option::single_float:
          try_to_set_value.operator()<float>();
          break;

        case option::single_double:
          try_to_set_value.operator()<double>();
          break;

        case option::single_string:
          try_to_set_value.operator()<std::string>();
          break;

        case option::multiple_bool:
          try_to_set_value.operator()<std::vector<bool>>();
          break;

        case option::multiple_int:
          try_to_set_value.operator()<std::vector<int>>();
          break;

        case option::multiple_size:
          try_to_set_value.operator()<std::vector<size_t>>();
          break;

        case option::multiple_float:
          try_to_set_value.operator()<std::vector<float>>();
          break;

        case option::multiple_double:
          try_to_set_value.operator()<std::vector<double>>();
          break;

        case option::multiple_string:
          try_to_set_value.operator()<std::vector<std::string>>();
          break;
        default:
          throw std::runtime_error(
              std::format("unkown option type {}", static_cast<int>(opt.opt_type)));
      }
    } catch (const result_error& ex) {
      if (ex.code() == error::INVALID_ARGUMENT) {
        throw std::runtime_error(std::format("invalid value format for {} option '{}': {}",
                                             option::type_name(opt.opt_type), opt_name, opt_value));
      }
      if (ex.code() == error::OUT_OF_RANGE) {
        throw std::runtime_error(std::format("the value is out of range for {} option '{}': {}",
                                             option::type_name(opt.opt_type), opt_name, opt_value));
      }
      throw;
    }
  }

  const app_info* _app_info;
  std::unordered_map<std::string, size_t> _search_idx;
  std::vector<option> _options;
  std::vector<std::string> _nonoptions;
  std::string _nonopt_name;
  bool _is_nonopt_required;
};

}  // namespace ascpp
