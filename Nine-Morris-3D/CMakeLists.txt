cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE GAME_HEADERS "src/*.hpp")

# Add WIN32 option for Windows in release + distribution mode
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NM3D_DISTRIBUTION_MODE)
        set(GUI_EXECUTABLE_WINDOWS WIN32)
    endif()
endif()

add_executable(Nine-Morris-3D "${GUI_EXECUTABLE_WINDOWS}" ${GAME_SOURCES} ${GAME_HEADERS})

target_include_directories(Nine-Morris-3D PRIVATE "src")

target_link_libraries(Nine-Morris-3D PRIVATE nine_morris_3d_engine)

if(UNIX)
    target_compile_options(Nine-Morris-3D PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(WIN32)
    target_compile_options(Nine-Morris-3D PRIVATE "/W4")
endif()

# Enable sanitizing in debug mode for GCC
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(UNIX AND NM3D_ASAN)
        target_compile_options(Nine-Morris-3D PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-g" "-fno-omit-frame-pointer")
        target_link_options(Nine-Morris-3D PRIVATE "-fsanitize=address" "-fsanitize=undefined")

        message(STATUS "Nine-Morris-3D: Building with sanitizers")
    endif()
endif()

# On Windows set Visual Studio working directory
set_property(TARGET Nine-Morris-3D PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
