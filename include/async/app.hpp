
#ifndef ASCPP_ASYNC_APP_HPP_
#define ASCPP_ASYNC_APP_HPP_

#include <chrono>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>

#include "asio/thread_pool.hpp"

namespace ascpp {

namespace fs = std::filesystem;                 // NOLINT
namespace ch = std::chrono;                     // NOLINT
using std::string_literals::operator""s;        // NOLINT
using std::string_view_literals::operator""sv;  // NOLINT
using std::chrono_literals::operator""ns;       // NOLINT
using std::chrono_literals::operator""us;       // NOLINT
using std::chrono_literals::operator""ms;       // NOLINT
using std::chrono_literals::operator""s;        // NOLINT
using std::chrono_literals::operator""min;      // NOLINT
using std::chrono_literals::operator""h;        // NOLINT

// TODO: 互斥
class App {
 public:
  App(std::string org_name, std::string app_name, std::string app_desc, std::string app_version)
      : org_name_{std::move(org_name)},
        app_name_{std::move(app_name)},
        app_desc_{std::move(app_desc)},
        app_version_{std::move(app_version)} {}

  App(App&&) = delete;
  App(const App&) = delete;
  auto operator=(App&&) -> App& = delete;
  auto operator=(const App&) -> App& = delete;
  ~App() = default;

  auto GetOrgName() -> const std::string& { return org_name_; }

  auto GetAppName() -> const std::string& { return app_name_; }

  auto GetAppDescription() -> const std::string& { return app_desc_; }

  auto GetAppVersion() -> const std::string& { return app_version_; }

  void Run() { thread_pool_.join(); }

  void Attach() { thread_pool_.attach(); }

  void Stop() { thread_pool_.stop(); }

  auto GetExecutor() -> asio::thread_pool::executor_type { return thread_pool_.get_executor(); }

 private:
  std::string org_name_;
  std::string app_name_;
  std::string app_desc_;
  std::string app_version_;
  asio::thread_pool thread_pool_;
};

}  // namespace ascpp

#endif  // !ASCPP_ASYNC_APP_HPP_
