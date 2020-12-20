find_file(
    DTYPE_STANDARD_LIBRARY
    NAMES dtypestd.o
    HINTS ${CMAKE_SOURCE_DIR}/lib/
    PATH_SUFFIXES D-Type/engine/libs/x86_64-osx_64-static/clang)

find_file(
    DTYPE_POWER_LIBRARY
    NAMES dtypepwr.o
    HINTS ${CMAKE_SOURCE_DIR}/lib/
    PATH_SUFFIXES D-Type/engine/libs/x86_64-osx_64-static/clang)

find_file(
    DTYPE_TEXT_LIBRARY
    NAMES dtypetxt.o
    HINTS ${CMAKE_SOURCE_DIR}/lib/
    PATH_SUFFIXES D-Type/engine/libs/x86_64-osx_64-static/clang)

find_library(
    DTYPE_TEXTLAYOUT_LIBRARY
    NAMES dtypelay.a
    HINTS ${CMAKE_SOURCE_DIR}/lib/
    PATH_SUFFIXES D-Type/engine/libs/x86_64-osx_64-static/clang)

list(APPEND DTYPE_LIBRARIES ${DTYPE_STANDARD_LIBRARY})
list(APPEND DTYPE_LIBRARIES ${DTYPE_POWER_LIBRARY})
list(APPEND DTYPE_LIBRARIES ${DTYPE_TEXT_LIBRARY})
list(APPEND DTYPE_LIBRARIES ${DTYPE_TEXTLAYOUT_LIBRARY})

find_path(
    DTYPECORE_INCLUDE_DIR
    NAMES dtype.h
    HINTS ${CMAKE_SOURCE_DIR}/lib/
    PATH_SUFFIXES D-Type/includes/core)

list(APPEND DTYPE_INCLUDE_DIRS ${DTYPECORE_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    dtype DEFAULT_MSG
    DTYPE_STANDARD_LIBRARY
    DTYPE_INCLUDE_DIRS)

mark_as_advanced(DTYPE_STANDARD_LIBRARY DTYPE_INCLUDE_DIRS)

if(DTYPE_STANDARD_LIBRARY AND NOT TARGET dtype::standard)
    add_library(dtype::standard OBJECT IMPORTED)
    set_target_properties(
        dtype::standard
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${DTYPE_INCLUDE_DIRS}"
        IMPORTED_OBJECTS ${DTYPE_STANDARD_LIBRARY})
endif()
if(DTYPE_POWER_LIBRARY AND NOT TARGET dtype::power)
    add_library(dtype::power OBJECT IMPORTED)
    set_target_properties(
        dtype::power
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${DTYPE_INCLUDE_DIRS}"
        IMPORTED_OBJECTS ${DTYPE_POWER_LIBRARY})
endif()
if(DTYPE_TEXT_LIBRARY AND NOT TARGET dtype::text)
    add_library(dtype::text OBJECT IMPORTED)
    set_target_properties(
        dtype::text
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${DTYPE_INCLUDE_DIRS}"
        IMPORTED_OBJECTS ${DTYPE_TEXT_LIBRARY})
endif()
if(DTYPE_TEXTLAYOUT_LIBRARY AND NOT TARGET dtype::textlayout)
    add_library(dtype::textlayout STATIC IMPORTED)
    set_target_properties(
        dtype::textlayout
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${DTYPE_INCLUDE_DIRS}"
        IMPORTED_LOCATION ${DTYPE_TEXT_LAYOUT_LIBRARY})
endif()

add_library(dtype::all INTERFACE IMPORTED)
set_property(TARGET dtype::all PROPERTY
  INTERFACE_LINK_LIBRARIES 
  dtype::standard
  dtype::power
  dtype::text
  dtype::textlayout)
