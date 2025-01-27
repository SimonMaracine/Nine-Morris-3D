cmake_minimum_required(VERSION 3.20)

add_library(nine_morris_3d_net_common INTERFACE
    "include/protocol.hpp"
)

target_include_directories(nine_morris_3d_net_common INTERFACE "include")

target_link_libraries(nine_morris_3d_net_common INTERFACE cereal)
