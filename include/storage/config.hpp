#ifndef ASCPP_STORAGE_CONFIG_H_
#define ASCPP_STORAGE_CONFIG_H_

#include <chrono>
#include <memory>
#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace ascpp {

// TODO: get, set, read(init), write(auto), sync(listen,notify)
class Configuration {
 public:
  Configuration(std::string org_name, std::string app_name);
  Configuration(Configuration&&) = default;
  Configuration(const Configuration&) = default;
  Configuration& operator=(Configuration&&) = default;
  Configuration& operator=(const Configuration&) = default;
  ~Configuration();

  template <typename T>
  T& get(const std::string& biz_name, const std::string& key);

  template <typename T>
  T* get_if(const std::string& biz_name, const std::string& key);

 private:
  std::string org_name_;
  std::string app_name_;
  std::chrono::system_clock::time_point last_modified_time_;
};

}  // namespace ascpp

#endif  // ASCPP_STORAGE_CONFIG_H_
