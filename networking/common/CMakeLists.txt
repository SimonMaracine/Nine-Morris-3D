cmake_minimum_required(VERSION 3.20)

add_library(networking_common STATIC
    "include/networking/internal/connection.hpp"
    "include/networking/internal/error.hpp"
    "include/networking/internal/message.hpp"
    "include/networking/internal/queue.hpp"
    "src/connection.cpp"
    "src/message.cpp"
)

target_include_directories(networking_common PUBLIC "include")

target_link_libraries(networking_common PUBLIC Boost::asio Boost::endian cereal)

target_compile_features(networking_common PRIVATE cxx_std_17)
set_target_properties(networking_common PROPERTIES CXX_EXTENSIONS OFF)

target_compile_definitions(networking_common PUBLIC
    "ASIO_NO_DEPRECATED"
    "_WIN32_WINNT=0x0601"
)
