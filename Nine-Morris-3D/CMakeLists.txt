cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE GAME_HEADERS "src/*.h")

# Add WIN32 option for Windows in release mode
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(GUI_EXECUTABLE_WINDOWS WIN32)
    endif()
endif()

add_executable(Nine-Morris-3D "${GUI_EXECUTABLE_WINDOWS}"
    ${GAME_SOURCES}
    ${GAME_HEADERS}
)

# Add all libraries
target_link_libraries(Nine-Morris-3D PRIVATE nine_morris_3d_engine)

# Add all include directories
target_include_directories(Nine-Morris-3D PRIVATE "src")

# Enable warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Nine-Morris-3D PRIVATE "-Wall" "-Wextra" "-Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Nine-Morris-3D PRIVATE "/W3")
endif()

# Enable sanitizing in debug mode for GCC
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(Nine-Morris-3D PRIVATE "-fsanitize=undefined" "-g")
        target_link_options(Nine-Morris-3D PRIVATE "-static-libasan" "-fsanitize=undefined")

        message(STATUS "Nine-Morris-3D: Building with sanitizers")
    endif()
endif()

# Use PCH
target_precompile_headers(Nine-Morris-3D PRIVATE "src/pch.hpp")

# On Windows set runtime library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET Nine-Morris-3D PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET Nine-Morris-3D PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# On Windows set Visual Studio working directory
set_property(TARGET Nine-Morris-3D PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
