cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

option(NM3D_FOR_DISTRIBUTION "Build for distribution" OFF)

project("Nine-Morris-3D" LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "Nine-Morris-3D: Project compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Nine-Morris-3D: Project build type: ${CMAKE_BUILD_TYPE}")

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Nine-Morris-3D: Compiler is not GCC or MSVC")
endif()

# Build the engine
add_subdirectory(nine_morris_3d_engine)

# Build the actual game
add_subdirectory(Nine-Morris-3D)

# Also build the encrypter
add_subdirectory(encrypter_helper)

message(STATUS "Nine-Morris-3D: Building for distribution: ${NM3D_FOR_DISTRIBUTION}")
