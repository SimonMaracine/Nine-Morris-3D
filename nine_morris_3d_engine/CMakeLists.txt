cmake_minimum_required(VERSION 3.20)

# Use static linking everywhere possible
set(BUILD_SHARED_LIBS OFF)

include(cmake/assimp.cmake)
include(cmake/dear_imgui.cmake)
include(cmake/glad.cmake)
include(cmake/sdl.cmake)
include(cmake/sdl_mixer.cmake)
include(cmake/stb.cmake)

add_subdirectory(extern/entt)
add_subdirectory(extern/glm)
add_subdirectory(extern/resmanager)
add_subdirectory(extern/utfcpp)

add_library(nine_morris_3d_engine STATIC
    "src/application/internal/file_system.cpp"
    "src/application/internal/input_codes.cpp"
    "src/application/internal/input.cpp"
    "src/application/internal/logging_base.cpp"
    "src/application/internal/task_manager.cpp"
    "src/application/internal/window.cpp"
    "src/application/application.cpp"
    "src/application/context.cpp"
    "src/application/crash.cpp"
    "src/application/scene.cpp"
    "src/audio/internal/audio.cpp"
    "src/audio/sound_data.cpp"
    "src/graphics/internal/debug_ui.cpp"
    "src/graphics/internal/imgui_context.cpp"
    "src/graphics/internal/opengl.cpp"
    "src/graphics/internal/renderer.cpp"
    "src/graphics/internal/shader_library.cpp"
    "src/graphics/opengl/buffer.cpp"
    "src/graphics/opengl/capabilities.cpp"
    "src/graphics/opengl/debug.cpp"
    "src/graphics/opengl/framebuffer.cpp"
    "src/graphics/opengl/shader.cpp"
    "src/graphics/opengl/texture.cpp"
    "src/graphics/opengl/vertex_array.cpp"
    "src/graphics/opengl/vertex_buffer_layout.cpp"
    "src/graphics/camera.cpp"
    "src/graphics/font.cpp"
    "src/graphics/material.cpp"
    "src/graphics/mesh.cpp"
    "src/graphics/post_processing_step.cpp"
    "src/graphics/scene.cpp"
    "src/graphics/texture_data.cpp"
    "src/other/internal/resources_cache.cpp"
    "src/other/dependencies.cpp"
    "src/other/localization.cpp"
    "src/other/utilities.cpp"

    "include/nine_morris_3d_engine/application/internal/error.hpp"
    "include/nine_morris_3d_engine/application/internal/event_dispatcher.hpp"
    "include/nine_morris_3d_engine/application/internal/file_system.hpp"
    "include/nine_morris_3d_engine/application/internal/input_codes.hpp"
    "include/nine_morris_3d_engine/application/internal/input.hpp"
    "include/nine_morris_3d_engine/application/internal/logging_base.hpp"
    "include/nine_morris_3d_engine/application/internal/task_manager.hpp"
    "include/nine_morris_3d_engine/application/internal/window.hpp"
    "include/nine_morris_3d_engine/application/application.hpp"
    "include/nine_morris_3d_engine/application/context.hpp"
    "include/nine_morris_3d_engine/application/crash.hpp"
    "include/nine_morris_3d_engine/application/events.hpp"
    "include/nine_morris_3d_engine/application/global_data.hpp"
    "include/nine_morris_3d_engine/application/id.hpp"
    "include/nine_morris_3d_engine/application/logging.hpp"
    "include/nine_morris_3d_engine/application/platform.hpp"
    "include/nine_morris_3d_engine/application/properties.hpp"
    "include/nine_morris_3d_engine/application/scene.hpp"
    "include/nine_morris_3d_engine/application/task.hpp"
    "include/nine_morris_3d_engine/audio/internal/audio.hpp"
    "include/nine_morris_3d_engine/audio/sound_data.hpp"
    "include/nine_morris_3d_engine/external/cereal.h++"
    "include/nine_morris_3d_engine/external/glm.h++"
    "include/nine_morris_3d_engine/external/imgui.h++"
    "include/nine_morris_3d_engine/external/process_asio.h++"
    "include/nine_morris_3d_engine/external/process_vfork_launcher.h++"
    "include/nine_morris_3d_engine/external/resmanager.h++"
    "include/nine_morris_3d_engine/nine_morris_3d.hpp"
    "include/nine_morris_3d_engine/graphics/internal/debug_ui.hpp"
    "include/nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
    "include/nine_morris_3d_engine/graphics/internal/opengl.hpp"
    "include/nine_morris_3d_engine/graphics/internal/post_processing_context.hpp"
    "include/nine_morris_3d_engine/graphics/internal/renderer.hpp"
    "include/nine_morris_3d_engine/graphics/internal/shader_library.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/buffer.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/capabilities.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/debug.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/shader.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/texture.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
    "include/nine_morris_3d_engine/graphics/opengl/vertex_buffer_layout.hpp"
    "include/nine_morris_3d_engine/graphics/camera.hpp"
    "include/nine_morris_3d_engine/graphics/font.hpp"
    "include/nine_morris_3d_engine/graphics/framebuffer_reader.hpp"
    "include/nine_morris_3d_engine/graphics/light.hpp"
    "include/nine_morris_3d_engine/graphics/material.hpp"
    "include/nine_morris_3d_engine/graphics/mesh.hpp"
    "include/nine_morris_3d_engine/graphics/post_processing_step.hpp"
    "include/nine_morris_3d_engine/graphics/scene.hpp"
    "include/nine_morris_3d_engine/graphics/skybox.hpp"
    "include/nine_morris_3d_engine/graphics/texture_data.hpp"
    "include/nine_morris_3d_engine/other/internal/array.hpp"
    "include/nine_morris_3d_engine/other/internal/default_camera_controller.hpp"
    "include/nine_morris_3d_engine/other/internal/resources_cache.hpp"
    "include/nine_morris_3d_engine/other/camera_controller.hpp"
    "include/nine_morris_3d_engine/other/dependencies.hpp"
    "include/nine_morris_3d_engine/other/localization.hpp"
    "include/nine_morris_3d_engine/other/utilities.hpp"
    "include/nine_morris_3d_engine/entry_point.hpp"
    "include/nine_morris_3d_engine/nine_morris_3d.hpp"
)

target_include_directories(nine_morris_3d_engine PUBLIC "include")

target_link_libraries(nine_morris_3d_engine PRIVATE
    glad
    assimp::assimp
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
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
    Boost::process
    Boost::asio
)

enable_warnings(nine_morris_3d_engine)
enable_sanitizers_debug_linux(nine_morris_3d_engine)

if(UNIX)
    target_compile_options(nine_morris_3d_engine PRIVATE "-Wno-parentheses")
endif()

# Use UTF-8 encoding on Windows
if(WIN32)
    target_compile_options(nine_morris_3d_engine PUBLIC "/utf-8")
endif()

target_compile_features(nine_morris_3d_engine PUBLIC cxx_std_20)
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
    "_WIN32_WINNT=0x0601"
)

target_compile_definitions(nine_morris_3d_engine PUBLIC
    "SPDLOG_ACTIVE_LEVEL=0"
    "GLM_ENABLE_EXPERIMENTAL"
    "_CRT_SECURE_NO_WARNINGS"
    "ASIO_NO_DEPRECATED"
)
