cmake_minimum_required(VERSION 3.20)

add_executable(nine_morris_3d_server
    "src/game_session.hpp"
    "src/main.cpp"
    "src/server.cpp"
    "src/server.hpp"
    "src/session_pool.cpp"
    "src/session_pool.hpp"
    "src/task_manager.cpp"
    "src/task_manager.hpp"
)

target_include_directories(nine_morris_3d_server PUBLIC "src")

target_link_libraries(nine_morris_3d_server PRIVATE networking_server nine_morris_3d_common)

enable_warnings(nine_morris_3d_server)
enable_sanitizers_debug_linux(nine_morris_3d_server)
