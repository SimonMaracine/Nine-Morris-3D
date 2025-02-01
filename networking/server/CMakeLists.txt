cmake_minimum_required(VERSION 3.20)

add_library(networking_server
    "include/networking/internal/client_connection.hpp"
    "include/networking/internal/id.hpp"
    "include/networking/internal/pool.hpp"
    "include/networking/server.hpp"
    "src/client_connection.cpp"
    "src/pool.cpp"
    "src/server.cpp"
)

target_include_directories(networking_server PUBLIC "include")

target_link_libraries(networking_server PUBLIC networking_common spdlog)

target_compile_features(networking_server PRIVATE cxx_std_17)
set_target_properties(networking_server PROPERTIES CXX_EXTENSIONS OFF)
