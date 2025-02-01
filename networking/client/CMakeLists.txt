cmake_minimum_required(VERSION 3.20)

add_library(networking_client STATIC
    "include/networking/internal/server_connection.hpp"
    "include/networking/client.hpp"
    "src/client.cpp"
    "src/server_connection.cpp"
)

target_include_directories(networking_client PUBLIC "include")

target_link_libraries(networking_client PUBLIC networking_common)

target_compile_features(networking_client PRIVATE cxx_std_17)
set_target_properties(networking_client PROPERTIES CXX_EXTENSIONS OFF)
