IF NOT EXIST build MKDIR build
cmake -DCMAKE_TOOLCHAIN_FILE=C:/Users/wesro/src/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -S . -B build 
