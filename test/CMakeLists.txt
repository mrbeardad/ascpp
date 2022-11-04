file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *_test.cpp)
add_executable(test ${SOURCES})
add_executable(ascpp::test ALIAS test)
target_link_libraries(test PRIVATE ascpp GTest::gtest_main)

add_executable(3rd_test 3rd.cpp)
target_link_libraries(3rd_test PRIVATE ascpp)
