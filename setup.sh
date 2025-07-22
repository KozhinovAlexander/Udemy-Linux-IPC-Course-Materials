#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root. Please use sudo."
    exit 1
fi

set -e

apt update
apt install libgtest-dev cmake ccache

# Build Google Test
cd /usr/src/gtest
cmake .
make
cp *.a /usr/lib
