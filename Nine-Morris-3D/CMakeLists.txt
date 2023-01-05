cmake_minimum_required(VERSION 3.20)

set(GAME Nine-Morris-3D)
set(ENGINE nine-morris-3d-engine)

file(GLOB_RECURSE GAME_HEADERS "src/*.h")
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Add WIN32 option for Windows in release mode
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(GUI_EXECUTABLE_WINDOWS WIN32)
    endif()
endif()

add_executable(${GAME} ${GUI_EXECUTABLE_WINDOWS}
    ${GAME_SOURCES}
    ${GAME_HEADERS}
)

# Enable warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${GAME} PRIVATE -Wall -Wextra -Wno-parentheses)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${GAME} PRIVATE /W2 /wd4244)
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

# Use PCH
target_precompile_headers(${GAME} PRIVATE "src/pch.hpp")

# Add all include directories
target_include_directories(${GAME} PRIVATE "src")

# Add all libraries
target_link_libraries(${GAME} PRIVATE
    ${ENGINE}
)

# Add compile definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${GAME} PRIVATE
        NM3D_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${GAME} PRIVATE
        NDEBUG
        ENTT_DISABLE_ASSERT
    )
endif()

# On Windows set runtime library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET
        ${GAME}
        PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET
        ${GAME}
        PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded
    )
endif()

# On Windows set Visual Studio working directory
set_property(TARGET ${GAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
