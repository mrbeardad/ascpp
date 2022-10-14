cmake_minimum_required(VERSION 3.16)
project(ascpp_examples VERSION 0.1)

configure_file(projects.hpp.in projects.hpp @ONLY)
add_executable(examples examples.cpp)
target_link_libraries(examples PRIVATE ascpp)
