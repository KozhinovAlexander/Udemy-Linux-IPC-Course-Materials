cmake_minimum_required(VERSION 3.10)
project(rtm_server LANGUAGES CXX)

# Create the shared library
add_library(rtm_server SHARED
  src/rtm_server.cpp
)
target_include_directories(rtm_server PUBLIC include)
target_link_libraries(rtm_server PUBLIC
  routing_table
)

# Create the executable
add_executable(rtm_server_runner
  src/main.cpp
)
target_link_libraries(rtm_server_runner PRIVATE
  rtm_server
)

# add_subdirectory(test)
