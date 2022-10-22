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
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_compile_definitions(ASIO_HAS_IO_URING=1)
endif()
include_directories(3rd_party/asio/asio/include)
include_directories(3rd_party/cxxopts/include)
add_compile_definitions(FMT_HEADER_ONLY=1)
include_directories(3rd_party/fmt/include)
include_directories(3rd_party/nlohmannjson/include)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
add_subdirectory(3rd_party/googletest)

# Targets
add_subdirectory(include)
add_subdirectory(test)
add_subdirectory(examples)

# Tartget: doc
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_EXCLUDE_PATTERNS */build/* */third_party/* */3rd_party/* */test/*)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
  set(DOXYGEN_HTML_OUTPUT ${CMAKE_BINARY_DIR}/doc)
  set(DOXYGEN_REFERENCED_BY_RELATION YES)
  set(DOXYGEN_REFERENCES_RELATION YES)
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_CALLER_GRAPH YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT png:cairo:cairo)
  set(DOXYGEN_UML_LOOK YES)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_EXTRACT_STATIC YES)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_SOURCE_BROWSER YES)
  set(DOXYGEN_STRIP_CODE_COMMENTS NO)
  set(DOXYGEN_SORT_MEMBER_DOCS NO)
  doxygen_add_docs(doc ${PROJECT_SOURCE_DIR} ALL)
endif()
