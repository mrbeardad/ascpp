// #include "spdlog/sinks/stdout_color_sinks.h"
// #include "spdlog/spdlog.h"

#include <filesystem>
#include <iostream>

auto main(int argc, const char** argv) -> int {
  // auto log = spdlog::stderr_color_st("fuck");
  // log->info("you");
  // std::cout << "fuck";
  std::filesystem::path p{"build\\"};
  std::cout << p.has_filename() << std::endl;
  return 0;
}
