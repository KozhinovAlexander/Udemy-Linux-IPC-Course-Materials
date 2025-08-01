cmake_minimum_required(VERSION 3.10)
project(rtm_server_client_tests)

find_package(GTest REQUIRED)
enable_testing()

set(UNIT_TEST rtm_server_client_tests)

add_executable(${UNIT_TEST}
  main.cpp
  test_rtm_server_client.cpp
)
target_link_libraries(${UNIT_TEST} PRIVATE
  ${GTEST_LIBRARIES}
  rtm_server
  rtm_client
)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
add_custom_command(
  TARGET ${UNIT_TEST}
  COMMENT "Run tests with GTest"
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${UNIT_TEST} --gtest_output=on_failure
)
