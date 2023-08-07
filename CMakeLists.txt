cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

project("Nine-Morris-3D"
    VERSION 0.4.0
    LANGUAGES C CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "NM3D: Project compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "NM3D: Project build type: ${CMAKE_BUILD_TYPE}")

# Build the engine
add_subdirectory(nine_morris_3d_engine)

# Build the actual game
add_subdirectory(Nine-Morris-3D)

# Also build the encrypter
add_subdirectory(encrypter_helper)
