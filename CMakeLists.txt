cmake_minimum_required(VERSION 3.15)

project(Briskgit C)

add_subdirectory(lib/sdl2)

add_executable(briskgit src/main.c)
target_link_libraries(briskgit LINK_PUBLIC SDL2)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(briskgit PRIVATE /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(briskgit PRIVATE -O2 -Wall -Wextra -pedantic -Werror)
endif()

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
