#!/usr/bin/bash

cd build
if [[ ! -f "CMakeCache.txt" ]]; then
   cmake -S ../src -B .
fi
cmake --build . -j 8
