cmake_minimum_required(VERSION 3.15)

project(Briskgit)

add_executable(briskgit 
               src/main.c
               src/app.h 
               src/app.c
               src/console.h 
               src/console.c
               src/profiler.h 
               src/profiler.c
               src/render.h 
               src/render.c
               src/text.h 
               src/text.c
               src/eva/eva.h)

#target_compile_definitions(briskgit PRIVATE PROFILER_ENABLED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(unofficial-glib CONFIG REQUIRED)
#find_package(ICU REQUIRED COMPONENTS uc dt in io)
find_library(PANGO_LIB pango-1.0)
find_library(PANGOFT2_LIB pangoft2-1.0)
find_library(FONTCONFIG_LIB fontconfig)
find_library(EXPAT_LIB expat)

target_link_libraries(briskgit PRIVATE 
                      ${PANGO_LIB}
                      ${PANGOFT2_LIB}
                      ${FONTCONFIG_LIB}
                      ${EXPAT_LIB}
                      freetype
                      harfbuzz::harfbuzz
                      unofficial::glib::glib
                      unofficial::glib::gmodule
                      unofficial::glib::gobject
                      )
                  #ICU::uc ICU::dt ICU::in ICU::io)

if (APPLE)
    enable_language(OBJC)
    target_sources(briskgit PRIVATE src/eva/eva_macos.m)

    target_link_libraries(briskgit PRIVATE
        "-framework Cocoa -framework Metal -framework MetalKit")

    configure_file(Info.plist Info.plist COPYONLY)
elseif(WIN32)
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
