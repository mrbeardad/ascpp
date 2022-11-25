file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *.cpp)
# TODO: convert to dll (SHARED)
add_library(ascpp ${SOURCES})
add_library(ascpp::ascpp ALIAS ascpp)
target_include_directories(ascpp PUBLIC .)
target_link_libraries(ascpp PUBLIC asio::asio fmt::fmt-header-only
                                   spdlog::spdlog nlohmann_json::nlohmann_json)
