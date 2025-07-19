#!/bin/bash

set -e

BUILD_DIR="build"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Run CMake to configure the project
cmake -S . -B "$BUILD_DIR"

# Build the project
cmake --build "$BUILD_DIR"
