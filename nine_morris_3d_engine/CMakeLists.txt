cmake_minimum_required(VERSION 3.20)

# Use static linking everywhere possible
set(BUILD_SHARED_LIBS OFF)

# Assimp
# set(USE_STATIC_CRT ON)  # FIXME
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)

# GLFW
# set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)  # FIXME
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

# OpenAL Soft
set(ALSOFT_UTILS OFF)
set(ALSOFT_NO_CONFIG_UTIL ON)
set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_INSTALL OFF)
set(ALSOFT_INSTALL_CONFIG OFF)
set(ALSOFT_INSTALL_HRTF_DATA OFF)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF)
set(ALSOFT_INSTALL_EXAMPLES OFF)
set(ALSOFT_INSTALL_UTILS OFF)
set(ALSOFT_UPDATE_BUILD_VERSION OFF)
set(LIBTYPE "STATIC")

include(cmake/cereal.cmake)
include(cmake/dear_imgui.cmake)
include(cmake/glad.cmake)
include(cmake/stb.cmake)

add_subdirectory(extern/assimp)
add_subdirectory(extern/entt)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/openal_soft)  # TODO be sure that system packages are available when building
add_subdirectory(extern/resmanager)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/utfcpp)

add_library(nine_morris_3d_engine STATIC
    "src/application/application.cpp"
    "src/application/capabilities.cpp"
    "src/application/context.cpp"
    "src/application/file_system.cpp"
    "src/application/input.cpp"
    "src/application/logging_base.cpp"
    "src/application/tasks.cpp"
    "src/application/window.cpp"
    "src/audio/openal/buffer.cpp"
    "src/audio/openal/debug.cpp"
    "src/audio/openal/listener.cpp"
    "src/audio/openal/source.cpp"
    "src/audio/context.cpp"
    "src/audio/music.cpp"
    "src/audio/sound_data.cpp"
    "src/graphics/opengl/buffer.cpp"
    "src/graphics/opengl/debug.cpp"
    "src/graphics/opengl/framebuffer.cpp"
    "src/graphics/opengl/opengl.cpp"
    "src/graphics/opengl/shader.cpp"
    "src/graphics/opengl/texture.cpp"
    "src/graphics/opengl/vertex_array.cpp"
    "src/graphics/opengl/vertex_buffer_layout.cpp"
    "src/graphics/camera.cpp"
    "src/graphics/debug_ui.cpp"
    "src/graphics/font.cpp"
    "src/graphics/imgui_context.cpp"
    "src/graphics/material.cpp"
    "src/graphics/mesh.cpp"
    "src/graphics/renderer.cpp"
    "src/graphics/scene.cpp"
    "src/graphics/shader_library.cpp"
    "src/graphics/texture_data.cpp"
    "src/other/dependencies.cpp"
    "src/other/random_gen.cpp"
    "src/other/resources_cache.cpp"
    "src/other/utilities.cpp"

    "include/nine_morris_3d_engine/application/application.hpp"
    "include/nine_morris_3d_engine/application/capabilities.hpp"
    "include/nine_morris_3d_engine/application/context.hpp"
    "include/nine_morris_3d_engine/application/entry_point.hpp"
    "include/nine_morris_3d_engine/application/error.hpp"
    "include/nine_morris_3d_engine/application/events.hpp"
    "include/nine_morris_3d_engine/application/file_system.hpp"
    "include/nine_morris_3d_engine/application/id.hpp"
    "include/nine_morris_3d_engine/application/input.hpp"
    "include/nine_morris_3d_engine/application/logging_base.hpp"
    "include/nine_morris_3d_engine/application/logging.hpp"
    "include/nine_morris_3d_engine/application/platform.hpp"
    "include/nine_morris_3d_engine/application/properties.hpp"
    "include/nine_morris_3d_engine/application/scene.hpp"
    "include/nine_morris_3d_engine/application/tasks.hpp"
    "include/nine_morris_3d_engine/application/window.hpp"
    "include/nine_morris_3d_engine/audio/openal/buffer.hpp"
    "include/nine_morris_3d_engine/audio/openal/debug.hpp"
    "include/nine_morris_3d_engine/audio/openal/listener.hpp"
    "include/nine_morris_3d_engine/audio/openal/source.hpp"
    "include/nine_morris_3d_engine/audio/context.hpp"
    "include/nine_morris_3d_engine/audio/music.hpp"
    "include/nine_morris_3d_engine/audio/sound_data.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/buffer.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/debug.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/opengl.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/shader.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/texture.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/vertex_buffer_layout.hpp"
    "include/nine_morris_3d_engine/graphics/camera.hpp"
    "include/nine_morris_3d_engine/graphics/debug_ui.hpp"
    "include/nine_morris_3d_engine/graphics/font.hpp"
    "include/nine_morris_3d_engine/graphics/framebuffer_reader.hpp"
    "include/nine_morris_3d_engine/graphics/imgui_context.hpp"
    "include/nine_morris_3d_engine/graphics/light.hpp"
    "include/nine_morris_3d_engine/graphics/material.hpp"
    "include/nine_morris_3d_engine/graphics/mesh.hpp"
    "include/nine_morris_3d_engine/graphics/post_processing_context.hpp"
    "include/nine_morris_3d_engine/graphics/post_processing_step.hpp"
    "include/nine_morris_3d_engine/graphics/renderable.hpp"
    "include/nine_morris_3d_engine/graphics/renderer.hpp"
    "include/nine_morris_3d_engine/graphics/scene.hpp"
    "include/nine_morris_3d_engine/graphics/shader_library.hpp"
    "include/nine_morris_3d_engine/graphics/texture_data.hpp"
    "include/nine_morris_3d_engine/other/camera_controller.hpp"
    "include/nine_morris_3d_engine/other/concurrent_loader.hpp"
    "include/nine_morris_3d_engine/other/dependencies.hpp"
    "include/nine_morris_3d_engine/other/random_gen.hpp"
    "include/nine_morris_3d_engine/other/resources_cache.hpp"
    "include/nine_morris_3d_engine/other/utilities.hpp"

    # For client
    "include/nine_morris_3d_engine/external/cereal.h++"
    "include/nine_morris_3d_engine/external/glm.h++"
    "include/nine_morris_3d_engine/external/imgui.h++"
    "include/nine_morris_3d_engine/external/resmanager.h++"
    "include/nine_morris_3d_engine/nine_morris_3d.hpp"
)

target_include_directories(nine_morris_3d_engine PUBLIC "include")

target_link_libraries(nine_morris_3d_engine PRIVATE
    glad
    assimp::assimp
    glfw
    OpenAL::OpenAL
    utf8cpp
    stb
)

target_link_libraries(nine_morris_3d_engine PUBLIC
    cereal
    EnTT::EnTT
    glm::glm
    dear_imgui
    resmanager
    spdlog
)

if(UNIX)
    target_compile_options(nine_morris_3d_engine PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(WIN32)
    target_compile_options(nine_morris_3d_engine PRIVATE "/W4")
endif()

# Enable sanitizing in debug mode for GCC
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(UNIX AND NM3D_ASAN)
        target_compile_options(nine_morris_3d_engine PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-g" "-fno-omit-frame-pointer")
        target_link_options(nine_morris_3d_engine PRIVATE "-fsanitize=address" "-fsanitize=undefined")
    endif()
endif()

# Use UTF-8 encoding on Windows
if(WIN32)
    target_compile_options(nine_morris_3d_engine PUBLIC "/utf-8")
endif()

target_compile_features(nine_morris_3d_engine PUBLIC cxx_std_17)
set_target_properties(nine_morris_3d_engine PROPERTIES CXX_EXTENSIONS OFF)

if(NM3D_DISTRIBUTION_MODE)
    target_compile_definitions(nine_morris_3d_engine PUBLIC
        "NM3D_DISTRIBUTION_MODE"  # Flag used by platform.hpp
        "NDEBUG"  # Disable asserts globally
        "ENTT_DISABLE_ASSERT"
    )
endif()

target_compile_definitions(nine_morris_3d_engine PRIVATE
    "UTF_CPP_CPLUSPLUS=201703"
    "AL_LIBTYPE_STATIC"
)

target_compile_definitions(nine_morris_3d_engine PUBLIC
    "SPDLOG_ACTIVE_LEVEL=0"
    "_CRT_SECURE_NO_WARNINGS"
    "GLM_ENABLE_EXPERIMENTAL"
)
