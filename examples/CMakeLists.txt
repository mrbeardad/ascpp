configure_file(projects.hpp.in projects.hpp @ONLY)
add_executable(examples examples.cpp)
add_executable(ascpp::examples ALIAS examples)
target_link_libraries(examples PRIVATE ascpp)
