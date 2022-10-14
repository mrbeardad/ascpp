#ifndef ASCPP_HPP_
#define ASCPP_HPP_

#include <memory>
#include <string>

namespace ascpp {

// TODO: 互斥
class Application {
  struct ApplicationImpl;

 public:
  static Application& GetInstance();

 private:
  Application(std::string org_name,
              std::string app_name,
              std::string app_desc,
              std::string app_version);
  Application(Application&&) = delete;
  Application(const Application&) = delete;
  Application& operator=(Application&&) = delete;
  Application& operator=(const Application&) = delete;
  ~Application();

 public:
  void Run();
  void Attach();
  void Stop();

 private:
  std::unique_ptr<ApplicationImpl> impl_;
};

}  // namespace ascpp

#endif  // !ASCPP_HPP_
