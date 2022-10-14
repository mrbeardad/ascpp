file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *.cpp)
add_executable(ascpp_test ${SOURCES})
target_link_libraries(ascpp_test ascpp GTest::gtest_main)
