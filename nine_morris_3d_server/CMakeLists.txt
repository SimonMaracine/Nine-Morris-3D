cmake_minimum_required(VERSION 3.20)

add_executable(nine_morris_3d_server
    "src/clock.cpp"
    "src/clock.hpp"
    "src/configuration.cpp"
    "src/configuration.hpp"
    "src/daemon.cpp"
    "src/daemon.hpp"
    "src/game_session.hpp"
    "src/main.cpp"
    "src/platform.hpp"
    "src/server.cpp"
    "src/server.hpp"
    "src/session_pool.cpp"
    "src/session_pool.hpp"
    "src/task_manager.cpp"
    "src/task_manager.hpp"
    "src/version.hpp"
)

target_include_directories(nine_morris_3d_server PUBLIC "src")

target_link_libraries(nine_morris_3d_server PRIVATE networking_server nine_morris_3d_common)

enable_warnings(nine_morris_3d_server)
enable_sanitizers_debug_linux(nine_morris_3d_server)

target_compile_features(nine_morris_3d_server PRIVATE cxx_std_17)
set_target_properties(nine_morris_3d_server PROPERTIES CXX_EXTENSIONS OFF)

if(UNIX AND NM3D_DISTRIBUTION_MODE)
    # Including systemd only makes sense for distribution
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBSYSTEMD REQUIRED "libsystemd")

    target_include_directories(nine_morris_3d_server PRIVATE ${LIBSYSTEMD_INCLUDE_DIRS})
    target_link_directories(nine_morris_3d_server PRIVATE ${LIBSYSTEMD_LIBRARY_DIRS})
    target_link_libraries(nine_morris_3d_server PRIVATE ${LIBSYSTEMD_LIBRARIES})

    message(STATUS "Nine-Morris-3D: Including support for systemd")
else()
    # Don't support Windows services
    target_compile_definitions(nine_morris_3d_server PRIVATE "SM_SERVER_NO_SYSTEMD")
endif()

if(NM3D_DISTRIBUTION_MODE)
    target_compile_definitions(nine_morris_3d_server PRIVATE "NM3D_DISTRIBUTION_MODE")
endif()
