cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE NM3D_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE NM3D_HEADERS "src/*.hpp")

# Add WIN32 option for Windows in release + distribution mode
if(WIN32)
    if(NM3D_DISTRIBUTION_MODE)
        set(GUI_EXECUTABLE_WINDOWS WIN32)
    endif()
endif()

add_executable(NineMorris3D ${GUI_EXECUTABLE_WINDOWS} ${NM3D_SOURCES} ${NM3D_HEADERS})

target_include_directories(NineMorris3D PRIVATE "src")

target_link_libraries(NineMorris3D PRIVATE nine_morris_3d_engine Boost::process)

enable_warnings(NineMorris3D)
enable_sanitizers_debug_linux(NineMorris3D)

# On Windows set Visual Studio working directory
set_property(TARGET NineMorris3D PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
