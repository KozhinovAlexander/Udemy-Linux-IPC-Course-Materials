cmake_minimum_required(VERSION 3.10)
project(routing_table_tests)

find_package(GTest REQUIRED)
enable_testing()

set(UNIT_TEST routing_table_tests)

add_executable(${UNIT_TEST}
  main.cpp
  test_routing_table.cpp
  test_routing_table_entry.cpp
)
target_link_libraries(${UNIT_TEST} PRIVATE
  ${GTEST_LIBRARIES}
  pthread
  routing_table
)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
add_custom_command(
  TARGET ${UNIT_TEST}
  COMMENT "Run tests with GTest"
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${UNIT_TEST} --gtest_output=on_failure
)
