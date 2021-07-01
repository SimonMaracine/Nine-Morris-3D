cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(EXECUTABLE_NAME "Nine-Morris-3D")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(FLTK_BUILD_TEST OFF)

project("Nine Morris 3D"
    VERSION 0.1.0
    LANGUAGES C CXX)

add_executable(${EXECUTABLE_NAME}
    "src/main.cpp"
    "src/logging.cpp"
    "src/opengl_canvas.cpp"
    "dependencies/Glad/src/glad.c")

# Enable warnings
target_compile_options(${EXECUTABLE_NAME} PUBLIC
    -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers)

# Use c++17
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_17)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

add_subdirectory(extern/spdlog)
add_subdirectory(extern/fltk)

# Add all include directories
target_include_directories(${EXECUTABLE_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${EXECUTABLE_NAME} SYSTEM PUBLIC
    "${CMAKE_SOURCE_DIR}/dependencies/Glad/include"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/fltk")

# Add all link directories
target_link_directories(${EXECUTABLE_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
    "${CMAKE_SOURCE_DIR}/build/extern/fltk/lib")

# Add all libraries
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    spdlog fltk fltk_gl)

# Add some defines
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building DEBUG mode")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=0)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message("Building RELEASE mode")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=6 NDEBUG)
endif()
