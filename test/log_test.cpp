#include <iostream>

#include "gtest/gtest.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

TEST(TestLog, Basic) {
  auto log = spdlog::basic_logger_st("basic", "test.log", true);
  log->info("loginfo");
  log->info("loginfo2");
}
