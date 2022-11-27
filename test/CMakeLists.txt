file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *_test.cpp)
add_executable(test_ascpp ${SOURCES})
add_executable(ascpp::test ALIAS test_ascpp)
target_link_libraries(test_ascpp PRIVATE ascpp GTest::gtest_main)
