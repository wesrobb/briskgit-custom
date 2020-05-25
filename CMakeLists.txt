cmake_minimum_required(VERSION 3.15)

project(Briskgit C OBJC)

add_executable(briskgit 
    src/main.c
    src/app.h 
    src/app.c
    src/profiler.h 
    src/profiler.c
    src/render.h 
    src/render.c)

target_include_directories(briskgit PRIVATE 
    ${PROJECT_SOURCE_DIR}/lib/freetype/macos/2.10.1/include
    ${PROJECT_SOURCE_DIR}/lib/harfbuzz/macos/2.6.6/include/harfbuzz
    ${PROJECT_SOURCE_DIR}/lib/sdl2/macos/2.0.12/include/SDL2)
target_link_libraries(briskgit LINK_PUBLIC
    ${PROJECT_SOURCE_DIR}/lib/freetype/macos/2.10.1/lib/libfreetype.a
    ${PROJECT_SOURCE_DIR}/lib/harfbuzz/macos/2.6.6/lib/libharfbuzz.a
    ${PROJECT_SOURCE_DIR}/lib/sdl2/macos/2.0.12/lib/libSDL2.a -lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(briskgit PRIVATE /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(briskgit PRIVATE -g -Wall -Wextra -pedantic -Werror -Wconversion)
endif()

configure_file(Info.plist Info.plist COPYONLY)

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
