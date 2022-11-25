#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <cctype>
#include <iostream>
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

#include "fmt/format.h"
#include "fmt/ranges.h"
#include "nlohmann/detail/meta/type_traits.hpp"

#include "app/info.hpp"
#include "utils/cmdline.hpp"
#include "utils/misc.hpp"

namespace ascpp {

template <typename T>
concept SingleOpt
    = std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, size_t>
      || std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, std::string>;

template <typename T>
concept MultiOpt
    = nlohmann::detail::is_specialization_of<std::vector, T>() && SingleOpt<typename T::value_type>;

template <typename T>
concept Opt = SingleOpt<T> || MultiOpt<T>;

namespace detail {

inline auto ToString(const std::string& s) -> const std::string& {
  return s;
}

template <SingleOpt T>
auto ToString(T t) -> std::string {
  return std::to_string(t);
}

template <MultiOpt T>
auto ToString(const T& v) -> std::string {
  return fmt::to_string(fmt::join(v, ","));
}

}  // namespace detail

struct Option {
  enum OptType {
    BOOL,
    INT,
    SIZE,
    FLOAT,
    DOUBLE,
    STRING,
    V_BOOL,
    V_INT,
    V_SIZE,
    V_FLOAT,
    V_DOUBLE,
    V_STRING
  };

  static inline auto MapTypeToEnum(const std::string& type_name) -> OptType {
    static auto map = std::unordered_map<std::string, OptType>{
        {typeid(bool).name(), BOOL},
        {typeid(int).name(), INT},
        {typeid(size_t).name(), SIZE},
        {typeid(float).name(), FLOAT},
        {typeid(double).name(), DOUBLE},
        {typeid(std::string).name(), STRING},
        {typeid(std::vector<bool>).name(), V_BOOL},
        {typeid(std::vector<int>).name(), V_INT},
        {typeid(std::vector<size_t>).name(), V_SIZE},
        {typeid(std::vector<float>).name(), V_FLOAT},
        {typeid(std::vector<double>).name(), V_DOUBLE},
        {typeid(std::vector<std::string>).name(), V_STRING},
    };
    return map.at(type_name);
  }

  static inline auto MapTypeToStr(OptType type) -> const char* {
    static auto map
        = std::array{"bool",           "int",           "size_t",         "float",
                     "double",         "string",        "list of bool",   "list of int",
                     "list of size_t", "list of float", "list of double", "list of string"};
    return map[static_cast<size_t>(type)];
  }

  template <SingleOpt T>
  auto CheckValue(T value) -> bool {
    if (!value_set.has_value()) {
      return true;
    }
    auto& set = std::any_cast<std::vector<T>&>(value_set);
    return std::ranges::binary_search(set, value) != set.end();
  }

  template <MultiOpt T>
  auto CheckValue(const T& values) -> bool {
    return CheckMultiValue(values) == values.end();
  }

  template <MultiOpt T>
  auto CheckMultiValue(const T& values) -> typename T::iterator {
    if (!value_set.has_value()) {
      return true;
    }
    auto& set = std::any_cast<T&>(value_set);
    return std::ranges::find(
        values, [&set](auto e) { return std::ranges::binary_search(set, e) == set.end(); });
  }

  OptType opt_type;
  std::string short_opt;
  std::string long_opt;
  std::string opt_desc;
  std::any value_set = {};
  std::any default_value = {};  ///< Also store the parsed value from command line arguments
  std::any implicit_value = {};
};

template <Opt T>
class OptionAdder {
 public:
  explicit OptionAdder(Option* opt) : opt_(opt) {}

  OptionAdder(OptionAdder&&) noexcept = default;
  OptionAdder(const OptionAdder&) = default;
  auto operator=(OptionAdder&&) noexcept -> OptionAdder& = default;
  auto operator=(const OptionAdder&) -> OptionAdder& = default;
  ~OptionAdder() = default;

  auto WithValueSet(std::unordered_set<T> limited_values) -> OptionAdder& {
    std::ranges::sort(limited_values);
    opt_->value_set = std::move(limited_values);
    return *this;
  }

  auto WithDefault(T default_value) -> OptionAdder& {
    auto& value_set = std::any_cast<std::unordered_set<T>&>(opt_->value_set);

    if constexpr (MultiOpt<T>) {
      if (auto itr = opt_->CheckMultiValue(default_value); itr != default_value.end()) {
        throw std::logic_error(fmt::format(
            "value set of option '{}' does not contaions the value '{}' in the default value",
            opt_->long_opt, *itr));
      }
    } else {
      if (!opt_->CheckValue(default_value)) {
        throw std::logic_error(
            fmt::format("value set of option '{}' does not contaions the default value: {}",
                        opt_->long_opt, default_value));
      }
    }
    opt_->default_value = std::move(default_value);
    return *this;
  }

  auto WithImplicit(T implicit_value) -> OptionAdder& {
    if constexpr (MultiOpt<T>) {
      if (auto itr = opt_->CheckMultiValue(implicit_value); itr != implicit_value.end()) {
        throw std::logic_error(fmt::format(
            "value set of option '{}' does not contaions the value '{}' in the implicit value",
            opt_->long_opt, *itr));
      }
    } else {
      if (!opt_->CheckValue(implicit_value)) {
        throw std::logic_error(
            fmt::format("value set of option '{}' does not contaions the implicit value: {}",
                        opt_->long_opt, implicit_value));
      }
    }
    opt_->implicit_value = std::move(implicit_value);
    return *this;
  }

 private:
  Option* opt_;
};

class Cmdline {
  template <Opt T>
  friend class OptionAdder;

 public:
  explicit Cmdline(const AppInfo* app_info) : app_info_(app_info) {}

  Cmdline(Cmdline&&) = default;
  Cmdline(const Cmdline&) = default;
  auto operator=(Cmdline&&) -> Cmdline& = default;
  auto operator=(const Cmdline&) -> Cmdline& = default;
  ~Cmdline() = default;

  template <Opt T>
  auto AddOption(char short_opt, std::string long_opt, std::string opt_desc) -> OptionAdder<T> {
    CheckShortOpt(short_opt);
    CheckLongOpt(long_opt);

    auto str_short_option = std::string{short_opt};
    search_idx_[str_short_option] = options_.size();
    search_idx_[long_opt] = options_.size();
    options_.emplace_back(Option{Option::MapTypeToEnum(typeid(T).name()),
                                 std::move(str_short_option), std::move(long_opt),
                                 std::move(opt_desc)});
    auto opt_adder = OptionAdder<T>(&options_.back());
    if constexpr (std::is_same_v<T, bool>) {
      opt_adder.WithDefault(false).WithImplicit(true);
    }
    return opt_adder;
  }

  template <Opt T>
  auto AddOption(std::string long_opt, std::string opt_desc) -> OptionAdder<T> {
    CheckLongOpt(long_opt);
    search_idx_[long_opt] = options_.size();
    options_.emplace_back(Option{Option::MapTypeToEnum(typeid(T).name()), "", std::move(long_opt),
                                 std::move(opt_desc)});

    auto opt_adder = OptionAdder<T>(&options_.back());
    if constexpr (std::is_same_v<T, bool>) {
      opt_adder.WithDefault(false).WithImplicit(true);
    }
    return opt_adder;
  }

  auto ParseArgs(int argc, const char* const argv[], bool silent = false) -> void {
    auto cerr_and_throw = [silent](const std::string& msg) {
      if (!silent) {
        std::cerr << msg;
      }
      throw std::runtime_error(msg);
    };
    auto end_parse = false;

    for (auto i = 1; i < argc; ++i) {
      auto this_arg = std::string(argv[i]);
      if (end_parse) {
        nonoptions_.emplace_back(this_arg);
      }
      if (this_arg == "--") {
        end_parse = true;
      } else if (this_arg.starts_with("--")) {
        auto es_pos = this_arg.find('=');
        auto opt_name = this_arg.substr(2, es_pos - 2);

        if (search_idx_.find(opt_name) == search_idx_.end()) {
          cerr_and_throw("no option: " + opt_name);
        }
        if (opt_name.size() < 2) {
          cerr_and_throw("wrong form for short option: " + this_arg);
        }

        if (es_pos == std::string::npos) {
          // form: --option [value]
          if (!HasImplicitValue(opt_name)) {
            if (i + 1 >= argc) {
              cerr_and_throw("requires a value for option: " + opt_name);
            }
            SetValue(opt_name, argv[++i]);
          } else {
            SetValueWithImplicit(opt_name);
          }
        } else {
          // form: --option=[value]
          if (es_pos + 1 >= this_arg.size()) {
            SetValue(opt_name, "");
          } else {
            SetValue(opt_name, this_arg.substr(es_pos + 1));
          }
        }
      } else if (this_arg.starts_with("-")) {
        for (auto j = 1ULL; j < this_arg.size(); ++j) {
          auto cur_opt = std::string{this_arg[j]};
          auto opt_idx = search_idx_.find(cur_opt);
          if (opt_idx == search_idx_.end()) {
            cerr_and_throw("no option: " + cur_opt);
          }
          if (!HasImplicitValue(cur_opt)) {
            if (j + 1 >= this_arg.size()) {
              // form: -opt value
              if (i + 1 >= argc) {
                cerr_and_throw("requires a value for option: " + cur_opt);
              }
              SetValue(cur_opt, argv[++i]);
            } else {
              // form: -optvalue
              SetValue(cur_opt, this_arg.substr(j + 1));
              break;
            }
          } else if (options_.at(search_idx_.at(cur_opt)).opt_type != Option::BOOL) {
            if (j + 1 >= this_arg.size()) {
              // form: -opt
              SetValueWithImplicit(cur_opt);
            } else {
              // form: -optvlaue
              SetValue(cur_opt, this_arg.substr(j + 1));
              break;
            }
          } else {
            SetValueWithImplicit(cur_opt);
          }
        }
      } else {
        nonoptions_.emplace_back(this_arg);
      }
    }

    for (auto& opt : options_) {
      if (!opt.default_value.has_value()) {
        cerr_and_throw("requires option: " + opt.long_opt);
      }
    }
  }

  template <Opt T>
  auto GetValue(const std::string& opt_name) -> PassT<T> {
    return std::any_cast<T&>(options_.at(search_idx_.at(opt_name)).default_value);
  }

 private:
  auto CheckShortOpt(char opt_name) const -> void {
    auto str_opt = std::string{opt_name};
    if (search_idx_.find(str_opt) != search_idx_.end()) {
      throw std::logic_error("duplicate option: " + str_opt);
    }
    if (auto debug = DebugGraphAnsiString(str_opt); debug != str_opt) {
      throw std::logic_error("short option must be a graphical character: " + debug);
    }
  }

  auto CheckLongOpt(const std::string& opt_name) const -> void {
    if (search_idx_.find(opt_name) != search_idx_.end()) {
      throw std::logic_error("duplicate option: " + opt_name);
    }
    if (auto debug = DebugGraphAnsiString(opt_name); debug != opt_name || opt_name.size() <= 2) {
      throw std::logic_error("long option must be graphical and have at least 2 character: "
                             + debug);
    }
    if (opt_name.find('=') != std::string::npos) {
      throw std::logic_error("long option name could not contain '='");
    }
  }

  auto HasNoImplicitValue(const std::string& opt_name) -> bool {
    return !options_.at(search_idx_.at(opt_name)).implicit_value.has_value();
  }

  auto HasImplicitValue(const std::string& opt_name) -> bool {
    auto& opt = options_.at(search_idx_.at(opt_name));
    return opt.implicit_value.has_value() && opt.opt_type != Option::BOOL;
  }

  auto SetValue(const std::string& opt_name, std::string opt_value) -> void {
    auto& opt = options_.at(search_idx_.at(opt_name));
    auto throw_when_invalid = [&opt_name, &opt_value, &opt]<typename T>(T& value) {
      if (opt.value_set.has_value()) {
        auto& set = std::any_cast<std::unordered_set<T>&>(opt.value_set);
        if (set.find(value) == set.end()) {
          throw std::runtime_error("invalid value for option '" + opt_name + "': " + opt_value);
        }
      }
    };

    try {
      switch (opt.opt_type) {
        case Option::BOOL: {
          if (opt_value.empty()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          std::ranges::for_each(opt_value, ::tolower);
          if (opt_value == "yes" || opt_value == "on" || opt_value == "true"
              || std::stoi(opt_value) == 1) {
            opt.default_value = true;
          } else if (opt_value == "no" || opt_value == "off" || opt_value == "false"
                     || std::stoi(opt_value) == 0) {
            opt.default_value = false;
          } else {
            throw std::runtime_error("invalid value for bool option '" + opt_name
                                     + "': " + opt_value);
          }
          break;
        }
        case Option::INT: {
          if (opt_value.empty()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          auto value = std::stoi(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::SIZE: {
          if (opt_value.empty()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          auto value = std::stoull(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::FLOAT: {
          if (opt_value.empty()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          auto value = std::stof(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::DOUBLE: {
          if (opt_value.empty()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          auto value = std::stod(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::STRING: {
          if (opt_value.empty()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          auto& value = opt_value;
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::V_BOOL: {
          if (!opt.default_value.has_value()) {
            SetValueWithImplicit(opt_name);
          }
          auto& vec = std::any_cast<std::vector<bool>&>(opt.default_value);
          if (opt_value.empty()) {
            break;
          }
          std::ranges::for_each(opt_value, ::tolower);
          if (opt_value == "yes" || opt_value == "on" || opt_value == "true"
              || std::stoi(opt_value) != 0) {
            vec.emplace_back(true);
          } else if (opt_value == "no" || opt_value == "off" || opt_value == "false"
                     || std::stoi(opt_value) == 0) {
            vec.emplace_back(false);
          } else {
            throw std::runtime_error("invalid value for bool option '" + opt_name
                                     + "': " + opt_value);
          }
          break;
        }
        case Option::V_INT: {
          if (!opt.default_value.has_value()) {
            SetValueWithImplicit(opt_name);
          }
          auto& vec = std::any_cast<std::vector<int>&>(opt.default_value);
          if (opt_value.empty()) {
            vec.emplace_back(0);
            break;
          }
          auto value = std::stoi(opt_value);
          throw_when_invalid(value);
          vec.emplace_back(value);
          break;
        }
        case Option::V_SIZE: {
          if (!opt.default_value.has_value()) {
            SetValueWithImplicit(opt_name);
          }
          auto& vec = std::any_cast<std::vector<size_t>&>(opt.default_value);
          if (opt_value.empty()) {
            vec.emplace_back(0);
            break;
          }
          auto value = std::stoull(opt_value);
          throw_when_invalid(value);
          vec.emplace_back(value);
          break;
        }
        case Option::V_FLOAT: {
          if (!opt.default_value.has_value()) {
            SetValueWithImplicit(opt_name);
          }
          auto& vec = std::any_cast<std::vector<float>&>(opt.default_value);
          if (opt_value.empty()) {
            vec.emplace_back(0);
            break;
          }
          auto value = std::stof(opt_value);
          throw_when_invalid(value);
          vec.emplace_back(value);
          break;
        }
        case Option::V_DOUBLE: {
          if (!opt.default_value.has_value()) {
            SetValueWithImplicit(opt_name);
          }
          auto& vec = std::any_cast<std::vector<double>&>(opt.default_value);
          if (opt_value.empty()) {
            vec.emplace_back(0);
            break;
          }
          auto value = std::stod(opt_value);
          throw_when_invalid(value);
          vec.emplace_back(value);
          break;
        }
        case Option::V_STRING: {
          // TODO: vec opt --vec=1,2,3,4 --vec=
          // TODO: duplicate? overwrite?
          if (!opt.default_value.has_value()) {
            SetValueWithImplicit(opt_name);
            break;
          }
          auto& vec = std::any_cast<std::vector<std::string>&>(opt.default_value);
          if (opt_value.empty()) {
            vec.emplace_back("");
            break;
          }
          break;
        }
      }
    } catch (std::invalid_argument const& ex) {
      throw std::runtime_error("invalid value '" + opt_value + "' format for option '" + opt_name
                               + "' that requires type " + Option::MapTypeToStr(opt.opt_type));
    } catch (std::out_of_range const& ex) {
      throw std::runtime_error("the given value '" + opt_value + "' is out of range for option '"
                               + opt_name + "' that requires type "
                               + Option::MapTypeToStr(opt.opt_type));
    }
  }

  auto SetValueWithImplicit(const std::string& opt_name) -> void {
    auto& opt = options_.at(search_idx_.at(opt_name));
    if (!opt.implicit_value.has_value()) {
      throw std::runtime_error("option '" + opt_name + "' has no implicit value");
    }
    opt.default_value = opt.implicit_value;
  }

  const AppInfo* app_info_;
  std::unordered_map<std::string, size_t> search_idx_;
  std::vector<Option> options_;
  std::vector<std::string> nonoptions_;
};

}  // namespace ascpp
