cmake_minimum_required(VERSION 3.20)

message("Binary dir: ${CMAKE_BINARY_DIR}")
message("Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
message("Source dir: ${CMAKE_SOURCE_DIR}")
message("Project binary dir: ${PROJECT_BINARY_DIR}")
message("Project source dir: ${PROJECT_SOURCE_DIR}")

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
    target_compile_options(${GAME} PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${GAME} PRIVATE /W2)
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
    # spdlog  # TODO this avoids having to link engine libraries publicly
)

# On Windows set runtime library
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
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
endif()

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

set_property(TARGET ${GAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
