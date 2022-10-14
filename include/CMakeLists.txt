add_library(ascpp INTERFACE)
add_library(ascpp::ascpp ALIAS ascpp)
target_include_directories(ascpp INTERFACE .)
