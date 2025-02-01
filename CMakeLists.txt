cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project("Nine-Morris-3D" LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

option(NM3D_DISTRIBUTION_MODE "Build for distribution" OFF)
option(NM3D_ASAN "Enable sanitizers" OFF)

if(NOT UNIX AND NOT WIN32)
    message(FATAL_ERROR "Nine-Morris-3D: Operating system is not Linux or Windows")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Nine-Morris-3D: Compiler is not GCC or MSVC")
endif()

if(NM3D_DISTRIBUTION_MODE AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "Nine-Morris-3D: Distribution mode is incompatible with debug mode")
endif()

function(enable_warnings target)
    if(UNIX)
        target_compile_options(${target} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
    elseif(WIN32)
        target_compile_options(${target} PRIVATE "/W4")
    endif()
endfunction()

function(enable_sanitizers_debug_linux target)
    # Enable sanitizing in debug mode for GCC
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(UNIX AND NM3D_ASAN)
            target_compile_options(${target} PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-g" "-fno-omit-frame-pointer")
            target_link_options(${target} PRIVATE "-fsanitize=address" "-fsanitize=undefined")
        endif()
    endif()
endfunction()

include(cmake/cereal.cmake)
include(cmake/boost.cmake)

add_subdirectory(extern/spdlog)
add_subdirectory(networking)

# Add the networking common
add_subdirectory(nine_morris_3d_common)

# Build the server
add_subdirectory(nine_morris_3d_server)

# Build the engine
add_subdirectory(nine_morris_3d_engine)

# Build the actual game
add_subdirectory(nine_morris_3d)

message(STATUS "Nine-Morris-3D: Project compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Nine-Morris-3D: Project build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Nine-Morris-3D: Building for distribution: ${NM3D_DISTRIBUTION_MODE}")
message(STATUS "Nine-Morris-3D: Building with sanitizers: ${NM3D_ASAN}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT nine_morris_3d)
