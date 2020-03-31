#!/bin/bash

# Cd into the suite
cd /opt/roboteam/roboteam_suite
# Pull each
git submodule foreach git pull
# Cd into correct repo
cd roboteam_world
# Checkout branch name
git checkout "$1"
# Cd back and create + cd into build
cd ..
mkdir build
cd build
# Cmake generate build files
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Ninja" ..
# Cmake build
cmake --build . --target world_tests -- -j $(nproc)
echo "Finished compiling"
# Run tests, xvfb-run simulates an xserver
cd roboteam_world
xvfb-run ./world_tests