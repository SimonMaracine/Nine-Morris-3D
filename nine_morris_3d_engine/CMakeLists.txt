cmake_minimum_required(VERSION 3.20)

option(NM3D_FOR_DISTRIBUTION "Build for distribution" OFF)

set(ENGINE "nine_morris_3d_engine")
set(GLAD_STB "glad_stb")
set(DEAR_IMGUI "dear_imgui")

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
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_DOCS OFF)

# cppblowfish
set(CPPBLOWFISH_BUILD_TESTS OFF)

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
set(LIBTYPE STATIC)  # To make static library

set(ENGINE_HEADERS
    "src/engine/application_base/application_builder.h"
    "src/engine/application_base/application_properties.h"
    "src/engine/application_base/application.h"
    "src/engine/application_base/capabilities.h"
    "src/engine/application_base/context.h"
    "src/engine/application_base/entry_point.h"
    "src/engine/application_base/events.h"
    "src/engine/application_base/input.h"
    "src/engine/application_base/panic.h"
    "src/engine/application_base/platform.h"
    "src/engine/application_base/window.h"
    "src/engine/audio/openal/buffer.h"
    "src/engine/audio/openal/info_and_debug.h"
    "src/engine/audio/openal/listener.h"
    "src/engine/audio/openal/source.h"
    "src/engine/audio/context.h"
    "src/engine/audio/music.h"
    "src/engine/audio/sound_data.h"
    "src/engine/dear_imgui/imgui_context.h"
    "src/engine/graphics/opengl/buffer.h"
    "src/engine/graphics/opengl/framebuffer.h"
    "src/engine/graphics/opengl/info_and_debug.h"
    "src/engine/graphics/opengl/shader.h"
    "src/engine/graphics/opengl/texture.h"
    "src/engine/graphics/opengl/vertex_array.h"
    "src/engine/graphics/renderer/gui_renderer.h"
    "src/engine/graphics/renderer/render_helpers.h"
    "src/engine/graphics/renderer/renderer.h"
    "src/engine/graphics/camera.h"
    "src/engine/graphics/font.h"
    "src/engine/graphics/framebuffer_reader.h"
    "src/engine/graphics/gui.h"
    "src/engine/graphics/identifier.h"
    "src/engine/graphics/light.h"
    "src/engine/graphics/material.h"
    "src/engine/graphics/post_processing.h"
    "src/engine/graphics/texture_data.h"
    "src/engine/graphics/vertex_buffer_layout.h"
    "src/engine/other/assert.h"
    "src/engine/other/camera_controller.h"
    "src/engine/other/concurrent_loader.h"
    "src/engine/other/dependencies.h"
    "src/engine/other/dummy.h"
    "src/engine/other/encrypt.h"
    "src/engine/other/file_system.h"
    "src/engine/other/logging.h"
    "src/engine/other/mesh.h"
    "src/engine/other/random_gen.h"
    "src/engine/other/resource_manager.h"
    "src/engine/scene/object_manager.h"
    "src/engine/scene/object.h"
    "src/engine/scene/renderables.h"
    "src/engine/scene/scene_list.h"
    "src/engine/scene/scene.h"

    # For client
    "src/engine/public/external/cereal.h"
    "src/engine/public/external/entt.h"
    "src/engine/public/external/glm.h"
    "src/engine/public/external/imgui.h"
    "src/engine/public/external/resmanager.h"
    "src/engine/public/application_base.h"
    "src/engine/public/audio.h"
    "src/engine/public/entry_point.h"
    "src/engine/public/graphics.h"
    "src/engine/public/other.h"
    "src/engine/public/prelude.h"  # Very common functionality # FIXME use this
    "src/engine/public/scene.h"
)

add_library(${ENGINE} STATIC
    "src/engine/application_base/application_builder.cpp"
    "src/engine/application_base/application.cpp"
    "src/engine/application_base/capabilities.cpp"
    "src/engine/application_base/context.cpp"
    "src/engine/application_base/input.cpp"
    "src/engine/application_base/panic.cpp"
    "src/engine/application_base/window.cpp"
    "src/engine/audio/openal/buffer.cpp"
    "src/engine/audio/openal/info_and_debug.cpp"
    "src/engine/audio/openal/listener.cpp"
    "src/engine/audio/openal/source.cpp"
    "src/engine/audio/context.cpp"
    "src/engine/audio/music.cpp"
    "src/engine/audio/sound_data.cpp"
    "src/engine/dear_imgui/imgui_context.cpp"
    "src/engine/graphics/opengl/buffer.cpp"
    "src/engine/graphics/opengl/framebuffer.cpp"
    "src/engine/graphics/opengl/info_and_debug.cpp"
    "src/engine/graphics/opengl/shader.cpp"
    "src/engine/graphics/opengl/texture.cpp"
    "src/engine/graphics/opengl/vertex_array.cpp"
    "src/engine/graphics/renderer/gui_renderer.cpp"
    "src/engine/graphics/renderer/render_helpers.cpp"
    "src/engine/graphics/renderer/renderer.cpp"
    "src/engine/graphics/camera.cpp"
    "src/engine/graphics/font.cpp"
    "src/engine/graphics/gui.cpp"
    "src/engine/graphics/identifier.cpp"
    "src/engine/graphics/material.cpp"
    "src/engine/graphics/texture_data.cpp"
    "src/engine/graphics/vertex_buffer_layout.cpp"
    "src/engine/other/dependencies.cpp"
    "src/engine/other/encrypt.cpp"
    "src/engine/other/file_system.cpp"
    "src/engine/other/logging.cpp"
    "src/engine/other/mesh.cpp"
    "src/engine/other/random_gen.cpp"
    "src/engine/other/resource_manager.cpp"
    "src/engine/scene/object_manager.cpp"
    "src/engine/scene/scene_list.cpp"
    ${ENGINE_HEADERS}
)

# Enable warnings only for engine
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${ENGINE} PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${ENGINE} PRIVATE /W2 /wd4244)
else()
    message(FATAL_ERROR "NM3D: Compiler is not GCC or MSVC")
endif()

# Enable sanitizing in debug mode for GCC
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${ENGINE} PRIVATE -fsanitize=undefined -g)
        target_link_options(${ENGINE} PRIVATE -static-libasan -fsanitize=undefined)

        message(STATUS "NM3D: Building engine with sanitizers")
    endif()
endif()

# Use UTF-8 encoding on Windows for both engine and client
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${ENGINE} PUBLIC /utf-8)
endif()

# Use C++17 for both engine and client
target_compile_features(${ENGINE} PUBLIC cxx_std_17)
set_target_properties(${ENGINE} PROPERTIES CXX_EXTENSIONS OFF)

# Use PCH
target_precompile_headers(${ENGINE} PRIVATE "src/pch.hpp")

# Make dependency target glad and stb
add_library(${GLAD_STB} STATIC
    "dependencies/glad/src/glad.c"
    "dependencies/stb_image/stb_image.c"
    "dependencies/stb_truetype/stb_truetype.c"
    "dependencies/stb_image_write/stb_image_write.c"
    "dependencies/stb_vorbis/stb_vorbis.c"
)

# Make dependency target Dear ImGui
add_library(${DEAR_IMGUI} STATIC
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
    "extern/imgui/backends/imgui_impl_glfw.cpp"
)

add_subdirectory(extern/assimp)
add_subdirectory(extern/cppblowfish)
add_subdirectory(extern/glfw)
add_subdirectory(extern/openal_soft)
add_subdirectory(extern/spdlog)

add_dependencies(${ENGINE} glfw)  # TODO this was necessary for some reason

# Add all include directories
target_include_directories(${ENGINE} PUBLIC "src")

# These have to be public in client
target_include_directories(${ENGINE} SYSTEM PUBLIC
    "dependencies/cereal/include"
    "dependencies/glad/include"
    "dependencies/stb_image"
    "dependencies/stb_truetype"
    "dependencies/stb_image_write"
    "dependencies/stb_vorbis"
    "extern/spdlog/include"
    "extern/assimp/include"
    "extern/glm"
    "extern/imgui"
    "extern/glfw/include"
    "extern/utfcpp/source"
    "extern/cppblowfish/include"
    "extern/entt/src"
    "extern/resmanager/src"
    "extern/openal_soft/include"
)

target_include_directories(${GLAD_STB} SYSTEM PRIVATE
    "dependencies/glad/include"
    "dependencies/stb_image"
    "dependencies/stb_truetype"
    "dependencies/stb_image_write"
)

target_include_directories(${DEAR_IMGUI} SYSTEM PRIVATE
    "extern/imgui"
    "extern/glfw/include"
)

# Add all libraries
target_link_libraries(${ENGINE} PRIVATE
    assimp
    cppblowfish
    glfw
    OpenAL::OpenAL
    ${GLAD_STB}
)

# But link these ones publicly
target_link_libraries(${ENGINE} PUBLIC
    spdlog
    ${DEAR_IMGUI}
)

# Add compile definitions for engine and client
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "NM3D: Building Nine Morris 3D engine in DEBUG mode")

    # Both engine and client
    target_compile_definitions(${ENGINE} PUBLIC
        "NM3D_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES"  # To easily enable or disable encrypted files
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "NM3D: Building Nine Morris 3D engine in RELEASE mode")

    # Optimization on in both engine and client
    target_compile_definitions(${ENGINE} PUBLIC
        "NDEBUG"  # Disable asserts globally
        "NM3D_RELEASE_BUILD_MODE"  # Just a flag used by platform.h
        "ENTT_DISABLE_ASSERT"
    )

    if(${NM3D_FOR_DISTRIBUTION})
        # Distribution flag on in both engine and client
        target_compile_definitions(${ENGINE} PUBLIC
            "NM3D_FOR_DISTRIBUTION"
        )

        message(STATUS "NM3D: Building Nine Morris 3D engine for DISTRIBUTION")
    endif()
else()
    message(FATAL_ERROR "NM3D: Unknown build type")
endif()

target_compile_definitions(${ENGINE} PRIVATE
    "IMGUI_IMPL_OPENGL_LOADER_GLAD"
    "UTF_CPP_CPLUSPLUS=201703"
    "AL_LIBTYPE_STATIC"
)

# Both engine and client
target_compile_definitions(${ENGINE} PUBLIC
    "SPDLOG_ACTIVE_LEVEL=0"
    "_CRT_SECURE_NO_WARNINGS"
    "ENTT_NO_ETO"
)

# On Windows set runtime library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET
        ${ENGINE} ${GLAD_STB} ${DEAR_IMGUI} spdlog assimp glfw cppblowfish OpenAL
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET
        ${ENGINE} ${GLAD_STB} ${DEAR_IMGUI} spdlog assimp glfw cppblowfish OpenAL
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()
