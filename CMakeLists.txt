# cmake general stuff
cmake_minimum_required(VERSION 3.20)
project(BRender)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/cmake")

# modules
include(projgen)
include(perltools)
if (MSVC)
    include(get-vs-includes)
endif()

# Options
option(BRENDER_STATIC "Build BRender as a static library" OFF)
option(BRENDER_BUILD_TOOLS "Build tools" OFF)
option(BRENDER_BUILD_EXAMPLES "Build samples" OFF)

# Platform detection
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "OS2")
    set(BRENDER_OS2 1)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "DOS")
    set(BRENDER_DOS 1)
    set(BRENDER_DOS_EXTENDER "DOS4GW")
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" OR WIN32)
    set(BRENDER_WIN32 1)
    add_definitions(-DWIN32=1)
else()
    message(FATAL_ERROR "Unsupported operative system")
endif()

# CPU detection
if ("${CMAKE_C_COMPILER_ARCHITECTURE_ID}" STREQUAL "X86")
    set(BRENDER_X86 1)
    add_definitions(-D__386__=1)
    #enable_language(ASM_NASM)
    enable_language(ASM_MASM)
else()
    message(FATAL_ERROR "Unsupported architecture")
endif()

# Compiler detection
if (MSVC)
    add_definitions(-D__VISUALC__=1)
endif()

# Global variables
add_definitions(-DSTATIC=static -DADD_RCS_ID=0 -DEVAL=0)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DPARANOID=1 -DDEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DPARANOID=0 -DDEBUG=0")

# Static/shared switch
if (BRENDER_STATIC)
    set(BRENDER_LIB_TYPE STATIC)
    set(BRENDER_LINK_TYPE PUBLIC)
    add_definitions(-DBR_STATIC=1)
else()
    set(BRENDER_LIB_TYPE SHARED)
    set(BRENDER_LINK_TYPE PRIVATE)
endif()

# Suffixes
set(BRENDER_FLOAT_SUFFIX f)
set(BRENDER_FIXED_SUFFIX x)
set(BRENDER_NORMAL_SUFFIX m)

if (NOT DEFINED BRENDER_FOLDER_PREFIX)
    set(BRENDER_FOLDER_PREFIX "BRender/")
endif()

# Include
set(BRENDER_GLOBAL_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/core/inc)

# Targets

add_subdirectory(core)
add_subdirectory(drivers)

if (BRENDER_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if (BRENDER_BUILD_EXAMPLES)
    add_subdirectory(samples)
endif()
