#!/bin/bash

# Cd into the suite
cd /opt/roboteam/roboteam_suite
# Make sure every branch is on master
git submodule foreach git checkout master
# Pull each
git submodule foreach git pull
# Cd into correct repo
cd roboteam_ai
# Checkout branch name
git checkout "$1"
# Cd back and create + cd into build
cd ..
mkdir build
cd build
# Cmake generate build files
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Ninja" ..
# Cmake build
cmake --build . --target ai_tests -- -j $(nproc)
echo "Finished compiling"
# Run tests, xvfb-run simulates an xserver
cd roboteam_ai
xvfb-run ./ai_tests