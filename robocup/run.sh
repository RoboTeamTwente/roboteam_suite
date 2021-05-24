#!/bin/bash

export BASE_DIR=/opt/roboteam/roboteam_suite/build
cd $BASE_DIR

# observer
cd ./roboteam_world
./roboteam_world &

# robothub
cd ../roboteam_robothub
./roboteam_robothub &

# AI
cd ../roboteam_ai
xvfb-run ./roboteam_ai