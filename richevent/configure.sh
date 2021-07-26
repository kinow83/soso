#!/bin/bash

# for ctest
# -DBUILD_TESTING=ON

cmake -B build -S .  $@
cmake --build build
