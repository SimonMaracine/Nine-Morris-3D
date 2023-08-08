cmake_minimum_required(VERSION 3.20)

# Use static linking everywhere possible
set(BUILD_SHARED_LIBS OFF)

# Assimp
set(USE_STATIC_CRT ON)  # Windows stuff
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)

# GLFW
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)  # Windows stuff
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

# OpenAL Soft
set(ALSOFT_UTILS OFF)
set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_INSTALL OFF)
set(ALSOFT_INSTALL_CONFIG OFF)
set(ALSOFT_INSTALL_HRTF_DATA OFF)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF)
set(ALSOFT_INSTALL_EXAMPLES OFF)
set(ALSOFT_INSTALL_UTILS OFF)
set(ALSOFT_UPDATE_BUILD_VERSION OFF)
set(LIBTYPE "STATIC")  # To make static library

# Make glad
add_library(glad STATIC "dependencies/glad/src/glad.c")

target_include_directories(glad PUBLIC "dependencies/glad/include")

# Make stb
add_library(stb STATIC
    "dependencies/stb_image/stb_image.c"
    "dependencies/stb_truetype/stb_truetype.c"
    "dependencies/stb_image_write/stb_image_write.c"
    "dependencies/stb_vorbis/stb_vorbis.c"
)

target_include_directories(stb PUBLIC
    "dependencies/stb_image"
    "dependencies/stb_truetype"
    "dependencies/stb_image_write"
    "dependencies/stb_vorbis"
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

# Make cereal
add_library(cereal INTERFACE)  # FIXME use submodule

target_include_directories(cereal INTERFACE "dependencies/cereal/include")

add_subdirectory(extern/assimp)
add_subdirectory(extern/cppblowfish)
add_subdirectory(extern/entt)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/openal_soft)
add_subdirectory(extern/resmanager)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/utfcpp)

set(ENGINE_HEADERS
    "include/engine/application_base/application_builder.hpp"
    "include/engine/application_base/application_properties.hpp"
    "include/engine/application_base/application.hpp"
    "include/engine/application_base/capabilities.hpp"
    "include/engine/application_base/context.hpp"
    "include/engine/application_base/entry_point.hpp"
    "include/engine/application_base/events.hpp"
    "include/engine/application_base/input.hpp"
    "include/engine/application_base/panic.hpp"
    "include/engine/application_base/platform.hpp"
    "include/engine/application_base/window.hpp"
    "include/engine/audio/openal/buffer.hpp"
    "include/engine/audio/openal/info_and_debug.hpp"
    "include/engine/audio/openal/listener.hpp"
    "include/engine/audio/openal/source.hpp"
    "include/engine/audio/context.hpp"
    "include/engine/audio/music.hpp"
    "include/engine/audio/sound_data.hpp"
    "include/engine/graphics/opengl/buffer.hpp"
    "include/engine/graphics/opengl/framebuffer.hpp"
    "include/engine/graphics/opengl/info_and_debug.hpp"
    "include/engine/graphics/opengl/shader.hpp"
    "include/engine/graphics/opengl/texture.hpp"
    "include/engine/graphics/opengl/vertex_array.hpp"
    "include/engine/graphics/renderer/gui_renderer.hpp"
    "include/engine/graphics/renderer/render_helpers.hpp"
    "include/engine/graphics/renderer/renderer.hpp"
    "include/engine/graphics/camera.hpp"
    "include/engine/graphics/font.hpp"
    "include/engine/graphics/framebuffer_reader.hpp"
    "include/engine/graphics/gui.hpp"
    "include/engine/graphics/identifier.hpp"
    "include/engine/graphics/imgui_context.hpp"
    "include/engine/graphics/light.hpp"
    "include/engine/graphics/material.hpp"
    "include/engine/graphics/post_processing.hpp"
    "include/engine/graphics/texture_data.hpp"
    "include/engine/graphics/vertex_buffer_layout.hpp"
    "include/engine/other/assert.hpp"
    "include/engine/other/camera_controller.hpp"
    "include/engine/other/concurrent_loader.hpp"
    "include/engine/other/dependencies.hpp"
    "include/engine/other/dummy.hpp"
    "include/engine/other/encrypt.hpp"
    "include/engine/other/file_system.hpp"
    "include/engine/other/logging.hpp"
    "include/engine/other/mesh.hpp"
    "include/engine/other/random_gen.hpp"
    "include/engine/other/resource_manager.hpp"
    "include/engine/scene/object_manager.hpp"
    "include/engine/scene/object.hpp"
    "include/engine/scene/renderables.hpp"
    "include/engine/scene/scene_list.hpp"
    "include/engine/scene/scene.hpp"

    # For client
    "include/engine/external/cereal.h++"
    "include/engine/external/entt.h++"
    "include/engine/external/glm.h++"
    "include/engine/external/imgui.h++"
    "include/engine/external/resmanager.h++"
    "include/engine/prelude.hpp"  # Very common functionality
)

add_library(nine_morris_3d_engine STATIC
    "src/application_base/application_builder.cpp"
    "src/application_base/application.cpp"
    "src/application_base/capabilities.cpp"
    "src/application_base/context.cpp"
    "src/application_base/input.cpp"
    "src/application_base/panic.cpp"
    "src/application_base/window.cpp"
    "src/audio/openal/buffer.cpp"
    "src/audio/openal/info_and_debug.cpp"
    "src/audio/openal/listener.cpp"
    "src/audio/openal/source.cpp"
    "src/audio/context.cpp"
    "src/audio/music.cpp"
    "src/audio/sound_data.cpp"
    "src/graphics/opengl/buffer.cpp"
    "src/graphics/opengl/framebuffer.cpp"
    "src/graphics/opengl/info_and_debug.cpp"
    "src/graphics/opengl/shader.cpp"
    "src/graphics/opengl/texture.cpp"
    "src/graphics/opengl/vertex_array.cpp"
    "src/graphics/renderer/gui_renderer.cpp"
    "src/graphics/renderer/render_helpers.cpp"
    "src/graphics/renderer/renderer.cpp"
    "src/graphics/camera.cpp"
    "src/graphics/font.cpp"
    "src/graphics/gui.cpp"
    "src/graphics/identifier.cpp"
    "src/graphics/imgui_context.cpp"
    "src/graphics/material.cpp"
    "src/graphics/texture_data.cpp"
    "src/graphics/vertex_buffer_layout.cpp"
    "src/other/dependencies.cpp"
    "src/other/encrypt.cpp"
    "src/other/file_system.cpp"
    "src/other/logging.cpp"
    "src/other/mesh.cpp"
    "src/other/random_gen.cpp"
    "src/other/resource_manager.cpp"
    "src/scene/object_manager.cpp"
    "src/scene/scene_list.cpp"
    ${ENGINE_HEADERS}
)

target_link_libraries(nine_morris_3d_engine PRIVATE
    glad
    stb
    cereal
    assimp
    cppblowfish
    EnTT::EnTT
    glfw
    glm
    OpenAL::OpenAL
    resmanager
    utf8cpp
)

target_link_libraries(nine_morris_3d_engine PUBLIC
    dear_imgui
    spdlog
)

target_include_directories(nine_morris_3d_engine PUBLIC "src")

# Enable warnings only for engine
if(UNIX)
    target_compile_options(nine_morris_3d_engine PRIVATE "-Wall" "-Wextra" "-Wpedantic")
elseif(WIN32)
    target_compile_options(nine_morris_3d_engine PRIVATE "/W3")
endif()

# Enable sanitizing in debug mode for GCC
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(UNIX)
        target_compile_options(nine_morris_3d_engine PRIVATE "-fsanitize=undefined" "-g")
        target_link_options(nine_morris_3d_engine PRIVATE "-static-libasan" "-fsanitize=undefined")

        message(STATUS "Nine-Morris-3D: Building engine with sanitizers")
    endif()
endif()

# Use UTF-8 encoding on Windows for both engine and client
if(WIN32)
    target_compile_options(nine_morris_3d_engine PUBLIC "/utf-8")
endif()

# Use C++ 17 for both engine and client
target_compile_features(nine_morris_3d_engine PUBLIC cxx_std_17)
set_target_properties(nine_morris_3d_engine PROPERTIES CXX_EXTENSIONS OFF)

# Add compile definitions for engine and client
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Optimization on in both engine and client
    target_compile_definitions(nine_morris_3d_engine PUBLIC
        "NDEBUG"  # Disable asserts globally
        "NM3D_RELEASE_MODE"  # Just a flag used by platform.h
        "ENTT_DISABLE_ASSERT"
    )

    if(NM3D_DISTRIBUTION_MODE)
        # Distribution flag on in both engine and client
        target_compile_definitions(nine_morris_3d_engine PUBLIC
            "NM3D_DISTRIBUTION_MODE"
        )

        message(STATUS "Nine-Morris-3D: Building Nine Morris 3D engine for DISTRIBUTION")
    endif()
endif()

target_compile_definitions(nine_morris_3d_engine PRIVATE
    "IMGUI_IMPL_OPENGL_LOADER_GLAD"
    "UTF_CPP_CPLUSPLUS=201703"
    "AL_LIBTYPE_STATIC"
)

# Both engine and client
target_compile_definitions(nine_morris_3d_engine PUBLIC
    "SPDLOG_ACTIVE_LEVEL=0"
    "_CRT_SECURE_NO_WARNINGS"
    "ENTT_NO_ETO"
)

# On Windows set runtime library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET
        nine_morris_3d_engine glad stb dear_imgui spdlog assimp glfw cppblowfish OpenAL
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET
        nine_morris_3d_engine glad stb dear_imgui spdlog assimp glfw cppblowfish OpenAL
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()
