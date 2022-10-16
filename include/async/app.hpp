
#ifndef ASCPP_ASYNC_APP_HPP_
#define ASCPP_ASYNC_APP_HPP_

#include <memory>
#include <string>

#include "asio.hpp"
#include "asio/thread_pool.hpp"

namespace ascpp {

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
  App& operator=(App&&) = delete;
  App& operator=(const App&) = delete;
  ~App() = default;

  void Run() { thread_pool_.join(); }

  void Attach() { thread_pool_.attach(); }

  void Stop() { thread_pool_.stop(); }

  asio::thread_pool::executor_type GetExecutor() { return thread_pool_.get_executor(); }

  const std::string& GetOrgName() { return org_name_; }

  const std::string& GetAppName() { return app_name_; }

  const std::string& GetAppDescription() { return app_desc_; }

  const std::string& GetAppVersion() { return app_version_; }

 private:
  std::string org_name_;
  std::string app_name_;
  std::string app_desc_;
  std::string app_version_;
  asio::thread_pool thread_pool_;
};

}  // namespace ascpp

#endif  // !ASCPP_ASYNC_APP_HPP_
