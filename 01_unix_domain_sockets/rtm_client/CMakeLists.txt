cmake_minimum_required(VERSION 3.10)
project(rtm_client LANGUAGES CXX)

# Create the shared library
add_library(rtm_client SHARED
  src/rtm_client.cpp
)
target_include_directories(rtm_client PUBLIC include)
target_link_libraries(rtm_client PUBLIC
  routing_table
)

# Create the executable
add_executable(rtm_client_runner
  src/main.cpp
)
target_link_libraries(rtm_client_runner PRIVATE
  rtm_client
)

# add_subdirectory(test)
