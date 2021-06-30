cmake_minimum_required(VERSION 3.10)

set(EXECUTABLE_NAME "Nine-Morris-3D")

project("Nine Morris 3D"
    VERSION 0.1.0
    LANGUAGES C CXX)

add_executable(${EXECUTABLE_NAME}
    "src/main.cpp"
    "dependencies/GLAD/src/glad.c")

# Enable warnings
target_compile_options(${EXECUTABLE_NAME} PUBLIC -Wall -Wextra)

# Use c++17
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_17)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# Add include directories
target_include_directories(${EXECUTABLE_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/dependencies/GLAD/include")

# Dirty way to link FLTK (for now; I should change that)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    -lfltk -lfltk_gl -lGLU -lGL -lXrender -lXcursor -lXfixes -lXext -lXft
    -lfontconfig -lXinerama -lpthread -ldl -lm -lX11)
