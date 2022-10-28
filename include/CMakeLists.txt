file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *.cpp)
add_library(ascpp ${SOURCES})
add_library(ascpp::ascpp ALIAS ascpp)
target_include_directories(ascpp PUBLIC .)
target_link_libraries(ascpp PUBLIC asio::asio cxxopts::cxxopts fmt::fmt
                                   nlohmann_json::nlohmann_json)
