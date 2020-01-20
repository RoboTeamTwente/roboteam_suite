#!/bin/bash

if [[ ! -f "/usr/local/lib/libgtest.a" ]]; then
    echo "GTest does not exist!";
fi

git clone https://github.com/RoboTeamTwente/roboteam_suit /opt/roboteam/roboteam_suite --recurse-submodules
cd /opt/roboteam/roboteam_suite
git submodule update --init --recursive
git submodule foreach git checkout master
cd roboteam_ai
git checkout "$1"
cd ..
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Ninja" ..
cmake --build . --target ai_tests -- -j 8
cd roboteam_ai
./ai_tests