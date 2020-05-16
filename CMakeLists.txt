cmake_minimum_required(VERSION 3.15)

project(Briskgit C OBJC)

add_subdirectory(lib/SDL2-2.0.12)
add_subdirectory(lib/freetype2)

set(FT_WITH_HARFBUZZ ON)

add_executable(briskgit 
    src/main.c
    src/app.h 
    src/app.c
    src/render.h 
    src/render.c
    )
target_link_libraries(briskgit LINK_PUBLIC SDL2-static freetype)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(briskgit PRIVATE /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(briskgit PRIVATE -g -Wall -Wextra -pedantic -Werror -Wconversion)
endif()

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
