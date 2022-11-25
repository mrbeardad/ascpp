# (asio::asio)
add_library(asio INTERFACE)
add_library(asio::asio ALIAS asio)
target_include_directories(asio INTERFACE asio/asio/include)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_definitions(asio INTERFACE ASIO_HAS_IO_URING=1)
endif()

# (fmt::fmt)
add_subdirectory(fmt) # TODO: migrate to std::format when it is mature enough

# (spdlog::spdlog)
set(SPDLOG_FMT_EXTERNAL_HO
    ON
    CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# (nlohmann_json::nlohmann_json)
add_subdirectory(nlohmannjson)

# (GTest::gtest_main)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
add_subdirectory(googletest)
