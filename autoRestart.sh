#!/bin/bash

while (true) do
   echo starting...
   ./cmake-build-debug/roboteam_ai/roboteam_ai 0
   exitcode=$?
   echo "exit code of command is $exitcode"
done