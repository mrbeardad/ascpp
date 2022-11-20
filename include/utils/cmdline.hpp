#pragma once

#include <any>
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
concept SingleOpt = std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, double>
                    || std::is_same_v<T, std::string>;

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
  std::string opt_type;
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

  auto WithValueSet(std::unordered_set<T> limited_values)
      -> std::enable_if_t<std::is_same_v<T, bool>, OptionAdder&> {
    opt_->value_set = std::move(limited_values);
    return *this;
  }

  auto WithDefault(T default_value) -> std::enable_if_t<std::is_same_v<T, bool>, OptionAdder&> {
    if (opt_->value_set.has_value()) {
      auto& value_set = std::any_cast<std::unordered_set<T>&>(opt_->value_set);
      if (value_set.find(default_value) == value_set.end()) {
        throw std::logic_error("value set for option '" + opt_->long_opt
                               + "'does not contaions the default value: "
                               + detail::ToString(default_value));
      }
    }
    opt_->default_value = std::move(default_value);
    return *this;
  }

  auto WithImplicit(T implicit_value) -> std::enable_if_t<std::is_same_v<T, bool>, OptionAdder&> {
    if (opt_->value_set.has_value()) {
      auto& value_set = std::any_cast<std::unordered_set<T>&>(opt_->value_set);
      if (value_set.find(implicit_value) == value_set.end()) {
        throw std::logic_error("value set for option '" + opt_->long_opt
                               + "'does not contaions the implicit value: "
                               + detail::ToString(implicit_value));
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
    auto str_short_option = std::string{short_opt};
    CheckShortOpt(str_short_option);
    CheckLongOpt(long_opt);
    search_idx_[str_short_option] = options_.size();
    search_idx_[long_opt] = options_.size();
    options_.emplace_back(Option{typeid(T).name(), std::move(str_short_option), std::move(long_opt),
                                 std::move(opt_desc)});
    return {&options_.back()};
  }

  template <Opt T>
  auto AddOption(std::string long_opt, std::string opt_desc) -> OptionAdder<T> {
    CheckLongOpt(long_opt);
    search_idx_[long_opt] = options_.size();
    options_.emplace_back(Option{typeid(T).name(), "", std::move(long_opt), std::move(opt_desc)});
    return {&options_.back()};
  }

  auto ParseArgs(int argc, const char* const argv[], bool silent = false) -> void {
    auto cerr_and_throw = [silent](const std::string& msg) {
      if (!silent) {
        std::cerr << msg;
      }
      throw std::runtime_error(msg);
    };
    auto end_parse = false;

    for (auto i = 0; i < argc; ++i) {
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
          if (Required(opt_name)) {
            if (i + 1 >= argc) {
              cerr_and_throw("requires a value for option: " + opt_name);
            }
            SetValue(opt_name, argv[++i]);
          } else {
            SetValueWithImplicit(opt_name);
          }
        } else {
          // form: --option=[value]
          if (Required(opt_name) || Optional(opt_name)) {
            if (es_pos + 1 >= this_arg.size()) {
              SetValue(opt_name, "");
            } else {
              SetValue(opt_name, this_arg.substr(es_pos + 1));
            }
          } else {
            cerr_and_throw("wrong form for bool option: " + this_arg);
          }
        }
      } else if (this_arg.starts_with("-")) {
        for (auto j = 1; j < this_arg.size(); ++j) {
          auto cur_opt = std::string{this_arg[j]};
          auto opt_idx = search_idx_.find(cur_opt);
          if (opt_idx == search_idx_.end()) {
            cerr_and_throw("no option: " + cur_opt);
          }
          if (Required(cur_opt)) {
            if (j + 1 >= this_arg.size()) {
              if (i + 1 >= argc) {
                cerr_and_throw("requires a value for option: " + cur_opt);
              }
              SetValue(cur_opt, argv[++i]);
              break;
            } else {
              SetValue(cur_opt, this_arg.substr(j + 1));
              break;
            }
          } else if (Optional(cur_opt)) {
            if (j + 1 >= this_arg.size()) {
              SetValueWithImplicit(cur_opt);
            } else {
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

 private:
  auto CheckShortOpt(const std::string& opt_name) const -> void {
    if (search_idx_.find(opt_name) != search_idx_.end()) {
      throw std::logic_error("duplicate option: " + opt_name);
    }
    if (auto debug = DebugGraphAnsiString(opt_name); debug != opt_name) {
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

  auto Required(const std::string& opt_name) -> bool {
    return !options_.at(search_idx_.at(opt_name)).implicit_value.has_value();
  }

  auto Optional(const std::string& opt_name) -> bool {
    auto& opt = options_.at(search_idx_.at(opt_name));
    return opt.implicit_value.has_value() && opt.opt_type != typeid(bool).name();
  }

  auto NoValue(const std::string& opt_name) -> bool {
    return options_.at(search_idx_.at(opt_name)).opt_type == typeid(bool).name();
  }

  auto SetValue(const std::string& opt_name, std::string opt_value) -> void {
    auto& any_value = options_.at(search_idx_.at(opt_name));
    // TODO:
  }

  auto SetValueWithImplicit(const std::string& opt_name) -> void {
    // TODO:
  }

  auto HasValue() -> bool {
    // TODO:
  }

  const AppInfo* app_info_;
  std::unordered_map<std::string, size_t> search_idx_;
  std::vector<Option> options_;
  std::vector<std::string> nonoptions_;
};

}  // namespace ascpp
