#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <cctype>
#include <cstddef>
#include <functional>
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

#include "fmt/core.h"
#include "fmt/format.h"
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
concept MultiOpt = is_specialization_v<std::vector, T> && SingleOpt<typename T::value_type>;

template <typename T>
concept Opt = SingleOpt<T> || MultiOpt<T>;

struct Option {
  // Use enum instead of std::any::type, so switch is available in runtime
  enum OptType {
    S_BOOL,
    S_INT,
    S_SIZE,
    S_FLOAT,
    S_DOUBLE,
    S_STRING,
    M_BOOL,
    M_INT,
    M_SIZE,
    M_FLOAT,
    M_DOUBLE,
    M_STRING
  };

  template <Opt T>
  static consteval auto MapTypeToEnum() -> OptType {
    if constexpr (std::is_same_v<T, bool>) {
      return S_BOOL;
    }
    if constexpr (std::is_same_v<T, int>) {
      return S_INT;
    }
    if constexpr (std::is_same_v<T, size_t>) {
      return S_SIZE;
    }
    if constexpr (std::is_same_v<T, float>) {
      return S_FLOAT;
    }
    if constexpr (std::is_same_v<T, double>) {
      return S_DOUBLE;
    }
    if constexpr (std::is_same_v<T, std::string>) {
      return S_STRING;
    }
    if constexpr (std::is_same_v<T, std::vector<bool>>) {
      return M_BOOL;
    }
    if constexpr (std::is_same_v<T, std::vector<int>>) {
      return M_INT;
    }
    if constexpr (std::is_same_v<T, std::vector<size_t>>) {
      return M_SIZE;
    }
    if constexpr (std::is_same_v<T, std::vector<float>>) {
      return M_FLOAT;
    }
    if constexpr (std::is_same_v<T, std::vector<double>>) {
      return M_DOUBLE;
    }
    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
      return M_STRING;
    }
    static_assert(true, "wrong type for option");
  }

  static inline auto MapTypeEnumToStr(OptType type) -> const char* {
    static auto map
        = std::array{"bool",           "int",           "size_t",         "float",
                     "double",         "string",        "list of bool",   "list of int",
                     "list of size_t", "list of float", "list of double", "list of string"};
    return map[static_cast<size_t>(type)];
  }

  template <SingleOpt T>
  auto CheckValue(const T& value) -> bool {
    if (!limits.has_value()) {
      return true;
    }
    if (limits.type() == typeid(std::unordered_set<T>)) {
      return std::any_cast<std::unordered_set<T>&>(limits).contains(value);
    }
    return std::any_cast<std::function<bool(const T&)>&>(limits)(value);
  }

  template <MultiOpt T>
  auto CheckValue(const T& values) -> bool {
    return CheckMultiValue(values) == values.end();
  }

  template <MultiOpt T>
  auto CheckMultiValue(const T& values) -> typename T::iterator {
    if (!limits.has_value()) {
      return true;
    }
    if (limits.type() == typeid(std::unordered_set<typename T::value_type>)) {
      auto& set = std::any_cast<std::unordered_set<typename T::value_type>&>(limits);
      return std::ranges::find(values, [&set](auto e) { return !set.contains(e); });
    }
    auto& callable = std::any_cast<std::function<bool(const typename T::value_type&)&>>(limits);
    return std::ranges::find(values, [&callable](auto e) { return !callable(e); });
  }

  OptType opt_type;
  std::string short_opt;
  std::string long_opt;
  std::string opt_desc;
  std::any limits = {};         ///< Store the limit value set or limit callable
  std::any default_value = {};  ///< Also store the parsed value from command line arguments
  std::any implicit_value = {};
};

template <Opt T, typename = void>
class OptionAdder;

template <Opt T>
class OptionAdder<T, std::enable_if_t<SingleOpt<T>>> {
 public:
  explicit OptionAdder(Option* opt) : opt_(opt) {}

  OptionAdder(OptionAdder&&) noexcept = default;
  OptionAdder(const OptionAdder&) = default;
  auto operator=(OptionAdder&&) noexcept -> OptionAdder& = default;
  auto operator=(const OptionAdder&) -> OptionAdder& = default;
  ~OptionAdder() = default;

  auto WithLimits(std::unordered_set<T> limit_set) -> OptionAdder& {
    opt_->limits = std::move(limit_set);
    return *this;
  }

  auto WithLimits(std::function<bool(const T&)> callable) -> OptionAdder& {
    opt_->limits = std::move(callable);
    return *this;
  }

  auto WithDefault(T default_value) -> OptionAdder& {
    if (!opt_->CheckValue(default_value)) {
      throw std::logic_error(fmt::format("the default value is invalid for {} option '{}' : {}",
                                         Option::MapTypeEnumToStr(opt_->opt_type), opt_->long_opt,
                                         default_value));
    }
    opt_->default_value = std::move(default_value);
    return *this;
  }

  auto WithImplicit(T implicit_value) -> OptionAdder& {
    if (!opt_->CheckValue(implicit_value)) {
      throw std::logic_error(fmt::format("the implicit value is invalid for {} option '{}' : {}",
                                         Option::MapTypeEnumToStr(opt_->opt_type), opt_->long_opt,
                                         implicit_value));
    }
    opt_->implicit_value = std::move(implicit_value);
    return *this;
  }

 private:
  Option* opt_;
};

template <Opt T>
class OptionAdder<T, std::enable_if<MultiOpt<T>>> {
 public:
  using ValueType = typename T::value_type;

  explicit OptionAdder(Option* opt) : opt_(opt) {}

  OptionAdder(OptionAdder&&) noexcept = default;
  OptionAdder(const OptionAdder&) = default;
  auto operator=(OptionAdder&&) noexcept -> OptionAdder& = default;
  auto operator=(const OptionAdder&) -> OptionAdder& = default;
  ~OptionAdder() = default;

  auto WithLimit(std::unordered_set<ValueType> limit_set) -> OptionAdder& {
    opt_->limits = std::move(limit_set);
    return *this;
  }

  auto WithLimit(std::function<bool(const ValueType&)> callable) -> OptionAdder& {
    opt_->limits = std::move(callable);
    return *this;
  }

  auto WithDefault(T default_value) -> OptionAdder& {
    if (auto itr = opt_->CheckMultiValue(default_value); itr != default_value.end()) {
      throw std::logic_error(
          fmt::format("the value in default values is invalid for {} option '{}': {}",
                      Option::MapTypeEnumToStr(opt_->opt_type), opt_->long_opt, *itr));
    }
    opt_->default_value = std::move(default_value);
    return *this;
  }

  auto WithImplicit(T implicit_value) -> OptionAdder& {
    if (auto itr = opt_->CheckMultiValue(implicit_value); itr != implicit_value.end()) {
      throw std::logic_error(
          fmt::format("the value in implicit values is invalid for {} option '{}': {}",
                      Option::MapTypeEnumToStr(opt_->opt_type), opt_->long_opt, *itr));
    }
    opt_->implicit_value = std::move(implicit_value);
    return *this;
  }

 private:
  Option* opt_;
};

class Cmdline {
  template <Opt T, typename>
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
    options_.emplace_back(Option{Option::MapTypeToEnum<T>(), std::move(str_short_option),
                                 std::move(long_opt), std::move(opt_desc)});
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
    options_.emplace_back(
        Option{Option::MapTypeToEnum<T>(), "", std::move(long_opt), std::move(opt_desc)});

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
          cerr_and_throw("no option '" + opt_name + "'");
        }
        if (opt_name.size() < 2) {
          cerr_and_throw("wrong form for short option: " + this_arg);
        }

        if (es_pos == std::string::npos) {
          // form: --option [value]
          if (!HasImplicitValue(opt_name)) {
            if (i + 1 >= argc) {
              cerr_and_throw("requires a value for option '" + opt_name + "'");
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
            cerr_and_throw("no option '" + cur_opt + "'");
          }
          if (!HasImplicitValue(cur_opt)) {
            if (j + 1 >= this_arg.size()) {
              // form: -opt value
              if (i + 1 >= argc) {
                cerr_and_throw("requires a value for option '" + cur_opt + "'");
              }
              SetValue(cur_opt, argv[++i]);
            } else {
              // form: -optvalue
              SetValue(cur_opt, this_arg.substr(j + 1));
              break;
            }
          } else if (options_.at(search_idx_.at(cur_opt)).opt_type != Option::S_BOOL) {
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
        cerr_and_throw("requires option '" + opt.long_opt + "'");
      }
    }
  }

  template <Opt T>
  auto GetValue(const std::string& opt_name) -> const T& {
    return std::any_cast<T&>(options_.at(search_idx_.at(opt_name)).default_value);
  }

  template <Opt T>
  auto GetValue(char opt_name) -> const T& {
    return GetValue<T>({opt_name});
  }

 private:
  auto CheckShortOpt(char opt_name) const -> void {
    auto str_opt = std::string{opt_name};
    if (search_idx_.find(str_opt) != search_idx_.end()) {
      throw std::logic_error("duplicate option '" + str_opt + "'");
    }
    if (auto debug = DebugGraphAnsiString(str_opt); debug != str_opt) {
      throw std::logic_error("short option must be a graphical character: '" + debug + "'");
    }
  }

  auto CheckLongOpt(const std::string& opt_name) const -> void {
    if (search_idx_.find(opt_name) != search_idx_.end()) {
      throw std::logic_error("duplicate option '" + opt_name + "'");
    }
    if (auto debug = DebugGraphAnsiString(opt_name); debug != opt_name || opt_name.size() <= 2) {
      throw std::logic_error("long option must be graphical and have at least 2 character: '"
                             + debug + "'");
    }
    if (opt_name.find('=') != std::string::npos) {
      throw std::logic_error("long option name could not contain '='");
    }
  }

  auto HasImplicitValue(const std::string& opt_name) -> bool {
    return options_.at(search_idx_.at(opt_name)).implicit_value.has_value();
  }

  auto SetValue(const std::string& opt_name, std::string opt_value) -> void {
    if (opt_value.empty()) {
      SetValueWithImplicit(opt_name);
      return;
    }

    auto& opt = options_.at(search_idx_.at(opt_name));

    auto throw_when_invalid = [&opt_name, &opt_value, &opt]<typename T>(const T& value) {
      if constexpr (MultiOpt<T>) {
        if (auto itr = opt.CheckMultiValue(value); itr != value.end()) {
          throw std::logic_error(fmt::format("invalid value for {} option '{}': {}",
                                             Option::MapTypeEnumToStr(opt.opt_type), opt_name,
                                             *itr));
        }
      } else {
        if (!opt.CheckValue(value)) {
          throw std::logic_error(fmt::format("invalid value for {} option '{}': {}",
                                             Option::MapTypeEnumToStr(opt.opt_type), opt_name,
                                             opt_value));
        }
      }
    };

    auto map_to_bool = [&opt_name, &opt_value](std::string value) {
      std::ranges::for_each(value, ::tolower);
      if (value == "yes" || value == "on" || value == "true" || value == "1") {
        return true;
      }
      if (value == "no" || value == "off" || value == "false" || value == "0") {
        return false;
      }
      throw std::runtime_error("invalid value for bool option '" + opt_name + "': " + opt_value);
    };

    try {
      switch (opt.opt_type) {
        case Option::S_BOOL: {
          std::ranges::for_each(opt_value, ::tolower);
          opt.default_value = map_to_bool(opt_value);
          break;
        }
        case Option::S_INT: {
          auto value = std::stoi(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::S_SIZE: {
          auto value = static_cast<size_t>(std::stoull(opt_value));
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::S_FLOAT: {
          auto value = std::stof(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::S_DOUBLE: {
          auto value = std::stod(opt_value);
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::S_STRING: {
          auto& value = opt_value;
          throw_when_invalid(value);
          opt.default_value = value;
          break;
        }
        case Option::M_BOOL: {
          auto vec = std::vector<bool>();
          for (auto word : std::views::split(opt_value, ",")) {
            vec.emplace_back(map_to_bool({word.begin(), word.end()}));
          }
          opt.default_value = vec;
          break;
        }
        case Option::M_INT: {
          auto vec = std::vector<int>();
          for (auto word : std::views::split(opt_value, ",")) {
            auto value = std::stoi(std::string(word.begin(), word.end()));
            throw_when_invalid(value);
            vec.emplace_back(value);
          }
          opt.default_value = vec;
          break;
        }
        case Option::M_SIZE: {
          auto vec = std::vector<size_t>();
          for (auto word : std::views::split(opt_value, ",")) {
            auto value = std::stoull(std::string(word.begin(), word.end()));
            throw_when_invalid(value);
            vec.emplace_back(value);
          }
          opt.default_value = vec;
          break;
        }
        case Option::M_FLOAT: {
          auto vec = std::vector<float>();
          for (auto word : std::views::split(opt_value, ",")) {
            auto value = std::stof(std::string(word.begin(), word.end()));
            throw_when_invalid(value);
            vec.emplace_back(value);
          }
          opt.default_value = vec;
          break;
        }
        case Option::M_DOUBLE: {
          auto vec = std::vector<double>();
          for (auto word : std::views::split(opt_value, ",")) {
            auto value = std::stod(std::string(word.begin(), word.end()));
            throw_when_invalid(value);
            vec.emplace_back(value);
          }
          opt.default_value = vec;
          break;
        }
        case Option::M_STRING: {
          auto vec = std::vector<std::string>();
          for (auto word : std::views::split(opt_value, ",")) {
            auto value = std::string(word.begin(), word.end());
            throw_when_invalid(value);
            vec.emplace_back(value);
          }
          opt.default_value = vec;
          break;
        }
      }
    } catch (const std::invalid_argument& ex) {
      throw std::runtime_error(fmt::format("invalid value format for {} option '{}': {}",
                                           Option::MapTypeEnumToStr(opt.opt_type), opt_name,
                                           opt_value));
    } catch (const std::out_of_range& ex) {
      throw std::runtime_error(fmt::format("the value is out of range for {} option '{}': {}",
                                           Option::MapTypeEnumToStr(opt.opt_type), opt_name,
                                           opt_value));
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
