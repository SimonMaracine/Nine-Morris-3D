cmake_minimum_required(VERSION 3.20)

add_library(networking STATIC
    "include/networking/server.hpp"
    "src/server.cpp"
)

target_include_directories(networking PUBLIC "include")

target_link_libraries(networking PRIVATE Boost::asio cereal)

enable_warnings(networking)
enable_sanitizers_debug_linux(networking)
