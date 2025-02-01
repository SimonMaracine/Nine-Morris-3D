cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE NM3D_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE NM3D_HEADERS "src/*.hpp")

# Add WIN32 option for Windows in release + distribution mode
if(WIN32)
    if(NM3D_DISTRIBUTION_MODE)
        set(GUI_EXECUTABLE_WINDOWS WIN32)
    endif()
endif()

add_executable(nine_morris_3d ${GUI_EXECUTABLE_WINDOWS} ${NM3D_SOURCES} ${NM3D_HEADERS})

target_include_directories(nine_morris_3d PRIVATE "src")

target_link_libraries(nine_morris_3d PRIVATE nine_morris_3d_engine Boost::process networking_client nine_morris_3d_common)

enable_warnings(nine_morris_3d)
enable_sanitizers_debug_linux(nine_morris_3d)

# target_compile_definitions(nine_morris_3d PRIVATE "_WIN32_WINNT=0x0601")

# On Windows set Visual Studio working directory
set_property(TARGET nine_morris_3d PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
