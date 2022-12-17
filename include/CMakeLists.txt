add_library(ascpp INTERFACE)
add_library(ascpp::ascpp ALIAS ascpp)
target_include_directories(ascpp INTERFACE .)
target_link_libraries(ascpp INTERFACE asio::asio utf::utf
                                      nlohmann_json::nlohmann_json)

file(
  GLOB_RECURSE ASCPP_HPPS
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *.hpp)
target_sources(ascpp PUBLIC FILE_SET HEADERS FILES ${ASCPP_HPPS})
set_property(TARGET ascpp PROPERTY VERIFY_INTERFACE_HEADER_SETS TRUE)
set_property(
  SOURCE
  DIRECTORY .
  PROPERTY LANGUAGE CXX)
