#pragma once

#include "asio/thread_pool.hpp"

namespace ascpp {

// TODO: 互斥
class Worker {
 public:
  Worker();
  ~Worker();

  void Run() { thread_pool_.join(); }

  void Attach() { thread_pool_.attach(); }

  void Stop() { thread_pool_.stop(); }

  auto GetExecutor() -> asio::thread_pool::executor_type { return thread_pool_.executor(); }

 private:
  asio::thread_pool thread_pool_;
};

}  // namespace ascpp
