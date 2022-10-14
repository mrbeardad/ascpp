file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS *.cpp)
add_executable(test ${SOURCES})
add_executable(ascpp::test ALIAS test)
target_link_libraries(test ascpp GTest::gtest_main)
