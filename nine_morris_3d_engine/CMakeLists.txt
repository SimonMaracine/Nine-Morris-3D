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

# Make glad
add_library(glad STATIC "extern/glad/src/glad.c")
target_include_directories(glad PUBLIC "extern/glad/include")

# Make stb
add_library(stb STATIC
    "extern/stb_image/stb_image.c"
    "extern/stb_image_resize2/stb_image_resize2.c"
    "extern/stb_image_write/stb_image_write.c"
    "extern/stb_truetype/stb_truetype.c"
    "extern/stb_vorbis/stb_vorbis.c"
)

target_include_directories(stb PUBLIC
    "extern/stb_image"
    "extern/stb_image_resize2"
    "extern/stb_image_write"
    "extern/stb_truetype"
    "extern/stb_vorbis"
)

# Make Dear ImGui
add_library(dear_imgui STATIC
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui.cpp"
    "extern/imgui/imgui.h"
    "extern/imgui/imgui_internal.h"

    "extern/imgui/backends/imgui_impl_glfw.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
    "extern/imgui/backends/imgui_impl_glfw.h"
    "extern/imgui/backends/imgui_impl_opengl3.h"
)

target_include_directories(dear_imgui PUBLIC "extern/imgui")
target_include_directories(dear_imgui PRIVATE "extern/glfw/include")
target_compile_definitions(dear_imgui PRIVATE "NDEBUG")

# Make cereal
add_library(cereal INTERFACE)  # FIXME use submodule
target_include_directories(cereal INTERFACE "extern/cereal/include")

add_subdirectory(extern/assimp)
add_subdirectory(extern/entt)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/openal_soft)  # TODO be sure that system packages are available when building
add_subdirectory(extern/resmanager)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/utfcpp)

add_library(nine_morris_3d_engine STATIC
    "src/application_base/application.cpp"
    "src/application_base/capabilities.cpp"
    "src/application_base/context.cpp"
    "src/application_base/file_system.cpp"
    "src/application_base/input.cpp"
    "src/application_base/logging.cpp"
    "src/application_base/tasks.cpp"
    "src/application_base/window.cpp"
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
    "src/graphics/font.cpp"
    "src/graphics/imgui_context.cpp"
    "src/graphics/material.cpp"
    "src/graphics/renderer.cpp"
    "src/graphics/shader_library.cpp"
    "src/graphics/texture_data.cpp"
    "src/other/dependencies.cpp"
    "src/other/mesh.cpp"
    "src/other/random_gen.cpp"
    "src/other/resources_cache.cpp"
    "src/other/utilities.cpp"

    "include/engine/application_base/application.hpp"
    "include/engine/application_base/capabilities.hpp"
    "include/engine/application_base/context.hpp"
    "include/engine/application_base/entry_point.hpp"
    "include/engine/application_base/error.hpp"
    "include/engine/application_base/events.hpp"
    "include/engine/application_base/file_system.hpp"
    "include/engine/application_base/id.hpp"
    "include/engine/application_base/input.hpp"
    "include/engine/application_base/logging.hpp"
    "include/engine/application_base/platform.hpp"
    "include/engine/application_base/properties.hpp"
    "include/engine/application_base/tasks.hpp"
    "include/engine/application_base/window.hpp"
    "include/engine/audio/openal/buffer.hpp"
    "include/engine/audio/openal/debug.hpp"
    "include/engine/audio/openal/listener.hpp"
    "include/engine/audio/openal/source.hpp"
    "include/engine/audio/context.hpp"
    "include/engine/audio/music.hpp"
    "include/engine/audio/sound_data.hpp"
    "include/engine/graphics/opengl/buffer.hpp"
    "include/engine/graphics/opengl/debug.hpp"
    "include/engine/graphics/opengl/framebuffer.hpp"
    "include/engine/graphics/opengl/opengl.hpp"
    "include/engine/graphics/opengl/shader.hpp"
    "include/engine/graphics/opengl/texture.hpp"
    "include/engine/graphics/opengl/vertex_array.hpp"
    "include/engine/graphics/opengl/vertex_buffer_layout.hpp"
    "include/engine/graphics/camera_2d.hpp"
    "include/engine/graphics/camera.hpp"
    "include/engine/graphics/font.hpp"
    "include/engine/graphics/framebuffer_reader.hpp"
    "include/engine/graphics/imgui_context.hpp"
    "include/engine/graphics/light.hpp"
    "include/engine/graphics/material.hpp"
    "include/engine/graphics/post_processing.hpp"
    "include/engine/graphics/renderable.hpp"
    "include/engine/graphics/renderer.hpp"
    "include/engine/graphics/shader_library.hpp"
    "include/engine/graphics/texture_data.hpp"
    "include/engine/other/camera_controller.hpp"
    "include/engine/other/concurrent_loader.hpp"
    "include/engine/other/dependencies.hpp"
    "include/engine/other/identifier.hpp"
    "include/engine/other/mesh.hpp"
    "include/engine/other/random_gen.hpp"
    "include/engine/other/resources_cache.hpp"
    "include/engine/other/utilities.hpp"
    "include/engine/scene/scene.hpp"

    # For client
    "include/engine/external/cereal.h++"
    "include/engine/external/glm.h++"
    "include/engine/external/imgui.h++"
    "include/engine/external/resmanager.h++"
    "include/engine/nine_morris_3d.hpp"
)

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

target_include_directories(nine_morris_3d_engine PUBLIC "include")

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

        message(STATUS "Nine-Morris-3D: Building engine with sanitizers")
    endif()
endif()

# Use UTF-8 encoding on Windows
if(WIN32)
    target_compile_options(nine_morris_3d_engine PUBLIC "/utf-8")
endif()

target_compile_features(nine_morris_3d_engine PUBLIC cxx_std_17)
set_target_properties(nine_morris_3d_engine PROPERTIES CXX_EXTENSIONS OFF)

# Add compile definitions for engine and client and others
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Optimization on
    target_compile_definitions(nine_morris_3d_engine PUBLIC "NM3D_RELEASE_MODE")  # Just a flag used by platform.hpp

    if(NM3D_DISTRIBUTION_MODE)
        target_compile_definitions(nine_morris_3d_engine PUBLIC
            "NM3D_DISTRIBUTION_MODE"  # Another flag used by platform.hpp
            "NDEBUG"  # Disable asserts globally
            "ENTT_DISABLE_ASSERT"
        )
    endif()
endif()

target_compile_definitions(nine_morris_3d_engine PRIVATE
    "IMGUI_IMPL_OPENGL_LOADER_GLAD"
    "UTF_CPP_CPLUSPLUS=201703"
    "AL_LIBTYPE_STATIC"
)

target_compile_definitions(nine_morris_3d_engine PUBLIC
    "SPDLOG_ACTIVE_LEVEL=0"
    "_CRT_SECURE_NO_WARNINGS"
)
