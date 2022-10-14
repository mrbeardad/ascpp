#include "storage/config.hpp"
#include <utility>

namespace ascpp {

Configuration::Configuration(std::string org_name, std::string app_name)
    : org_name_{std::move(org_name)}, app_name_{std::move(app_name)} {}

Configuration::~Configuration() {}

}  // namespace ascpp
