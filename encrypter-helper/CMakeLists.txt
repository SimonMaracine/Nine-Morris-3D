cmake_minimum_required(VERSION 3.20)

set(EXECUTABLE_NAME "encrypter")
set(CPPBLOWFISH_BUILD_TESTS OFF)

project("encrypter-helper" LANGUAGES CXX)

add_executable(${EXECUTABLE_NAME} main.cpp)

add_subdirectory("../extern/cppblowfish" build)

target_include_directories(${EXECUTABLE_NAME} PRIVATE "../extern/cppblowfish/include")
target_link_libraries(${EXECUTABLE_NAME} PRIVATE cppblowfish)
