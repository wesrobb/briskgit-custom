#!/bin/bash
mkdir -p build
cmake -DCMAKE_TOOLCHAIN_FILE=/usr/local/Cellar/vcpkg/2020.04/libexec/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-osx -S . -B build 
