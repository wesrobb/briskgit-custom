cmake_minimum_required(VERSION 3.15)

project(Briskgit)

add_executable(briskgit 
               src/main.c
               src/app.h 
               src/app.c
               src/console.h 
               src/console.c
               src/hash.h 
               src/hash.c
               src/profiler.h 
               src/profiler.c
               src/rect.h 
               src/rect.c
               src/render.h 
               src/render.c
               src/text.h 
               src/text.c
               src/ustr.h 
               src/ustr.c
               src/vec2.h 
               src/eva/eva.h)

#target_compile_definitions(briskgit PRIVATE PROFILER_ENABLED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc dt in io)

target_link_libraries(briskgit PRIVATE 
                      freetype
                      harfbuzz::harfbuzz
                      ICU::uc ICU::dt ICU::in ICU::io)

if (APPLE)
    target_compile_definitions(briskgit PRIVATE BG_MACOS)
    enable_language(OBJC)
    target_sources(briskgit PRIVATE src/eva/eva_macos.m)

    target_link_libraries(briskgit PRIVATE
        "-framework Cocoa -framework Metal -framework MetalKit -framework CoreText")

    configure_file(Info.plist Info.plist COPYONLY)
elseif(WIN32)
    target_compile_definitions(briskgit PRIVATE BG_WINDOWS)
    target_sources(briskgit PRIVATE src/eva/eva_windows.c)

endif()

if (MSVC)
    # Use statically linked CRT on Windows
    set_property(TARGET briskgit PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # warning level 4 and all warnings as errors
    target_compile_options(briskgit PRIVATE /W4 /WX /wd4201 /wd4204)

    # The MSVC generator puts the built exe in build/Debug (or build/Release).
    # This post build step copies the latest built exe to the root of the 
    # build folder in order to make it easy to run via a shortcut.
    add_custom_command(TARGET briskgit POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:briskgit> ${CMAKE_BINARY_DIR})
else()
    # lots of warnings and all warnings as errors
    target_compile_options(briskgit PRIVATE -g -Wall -Wextra -pedantic -Wconversion)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
