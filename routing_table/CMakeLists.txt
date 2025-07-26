cmake_minimum_required(VERSION 3.10)
project(routing_table LANGUAGES CXX)

# Create the shared library
add_library(routing_table SHARED
  src/routing_table.cpp
)
target_include_directories(routing_table PUBLIC include)

add_subdirectory(test)
