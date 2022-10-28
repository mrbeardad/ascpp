# (asio::asio) async framework
add_library(asio INTERFACE)
add_library(asio::asio ALIAS asio)
target_include_directories(asio INTERFACE asio/asio/include)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_definitions(asio INTERFACE ASIO_HAS_IO_URING=1)
endif()

# (cxxopts::cxxopts) command line arguments parser
add_subdirectory(cxxopts)

# (fmt::fmt) c++20 format syntax
add_subdirectory(fmt) # TODO: migrate to std::format when it is mature enough

# (nlohmann_json::nlohmann_json) json parser
add_subdirectory(nlohmannjson)

# (glog::glog) log
set(WITH_GFLAGS
    OFF
    CACHE BOOL "" FORCE)
add_subdirectory(glog)

# (GTest::gtest_main) unit test
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
add_subdirectory(googletest)
