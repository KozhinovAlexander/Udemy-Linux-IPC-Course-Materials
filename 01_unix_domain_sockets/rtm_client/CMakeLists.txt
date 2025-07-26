
cmake_minimum_required(VERSION 3.10)
project(rtm_client LANGUAGES CXX)

# Create the shared library
add_library(rtm_client SHARED
    src/main.cpp
    src/rtm_client.cpp
)
target_include_directories(rtm_client PUBLIC include)

# add_subdirectory(test)
