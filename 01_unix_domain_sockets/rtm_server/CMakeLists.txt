
cmake_minimum_required(VERSION 3.10)
project(rtm_server LANGUAGES CXX)

# Create the shared library
add_library(rtm_server SHARED
    src/main.cpp
    src/rtm_server.cpp
)
target_include_directories(rtm_server PUBLIC include)

# add_subdirectory(test)
