# Project
cmake_minimum_required(VERSION 3.16)
project(ascpp VERSION 0.1)

# Global compile options
include_directories(include)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(MSVC)
    add_compile_options(/W4)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -fstandalone-debug)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -g3)
  endif()
endif()

# Third party dependencies
include_directories(3rd_party/asio/asio/include)
include_directories(3rd_party/cxxopts/include)
include_directories(3rd_party/nlohmannjson/include)
include_directories(3rd_party/cpp-dirs/include)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
add_subdirectory(3rd_party/googletest)

# Targets
add_subdirectory(include)
add_subdirectory(test)
add_subdirectory(examples)
