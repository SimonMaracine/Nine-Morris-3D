cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(ENGINE nine-morris-3d-engine)
set(GAME Nine-Morris-3D)
set(GLAD_STB glad-stb)
set(MY_IMGUI my-imgui)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

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
set(LIBTYPE STATIC)  # To make static library

project("Nine-Morris-3D"
    VERSION 0.2.0
    LANGUAGES C CXX
)

file(GLOB_RECURSE GAME_HEADERS "src/*.h")
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Add WIN32 option for Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Release)
        set(NO_CONSOLE WIN32)
    endif()
endif()

add_executable(${GAME} ${NO_CONSOLE}
    ${GAME_SOURCES}
    ${GAME_HEADERS}
)

# On Windows tell linker to use main instead of WinMain
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_link_options(${GAME} PRIVATE
        /entry:mainCRTStartup
    )
endif()

set(ENGINE_HEADERS
    "engine/nine_morris_3d_engine/application/application_builder.h"
    "engine/nine_morris_3d_engine/application/application_data.h"
    "engine/nine_morris_3d_engine/application/application.h"
    "engine/nine_morris_3d_engine/application/capabilities.h"
    "engine/nine_morris_3d_engine/application/events.h"
    "engine/nine_morris_3d_engine/application/input.h"
    "engine/nine_morris_3d_engine/application/platform.h"
    "engine/nine_morris_3d_engine/application/scene.h"
    "engine/nine_morris_3d_engine/application/window.h"
    "engine/nine_morris_3d_engine/audio/openal/buffer.h"
    "engine/nine_morris_3d_engine/audio/openal/info_and_debug.h"
    "engine/nine_morris_3d_engine/audio/openal/listener.h"
    "engine/nine_morris_3d_engine/audio/openal/source.h"
    "engine/nine_morris_3d_engine/audio/context.h"
    "engine/nine_morris_3d_engine/audio/sound_data.h"
    "engine/nine_morris_3d_engine/graphics/opengl/buffer.h"
    "engine/nine_morris_3d_engine/graphics/opengl/framebuffer.h"
    "engine/nine_morris_3d_engine/graphics/opengl/info_and_debug.h"
    "engine/nine_morris_3d_engine/graphics/opengl/shader.h"
    "engine/nine_morris_3d_engine/graphics/opengl/texture.h"
    "engine/nine_morris_3d_engine/graphics/opengl/vertex_array.h"
    "engine/nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
    "engine/nine_morris_3d_engine/graphics/renderer/renderer.h"
    "engine/nine_morris_3d_engine/graphics/buffer_layout.h"
    "engine/nine_morris_3d_engine/graphics/camera.h"
    "engine/nine_morris_3d_engine/graphics/font.h"
    "engine/nine_morris_3d_engine/graphics/framebuffer_reader.h"
    "engine/nine_morris_3d_engine/graphics/identifier.h"
    "engine/nine_morris_3d_engine/graphics/imgui_context.h"
    "engine/nine_morris_3d_engine/graphics/light.h"
    "engine/nine_morris_3d_engine/graphics/material.h"
    "engine/nine_morris_3d_engine/graphics/post_processing.h"
    "engine/nine_morris_3d_engine/graphics/texture_data.h"
    "engine/nine_morris_3d_engine/other/assert.h"
    "engine/nine_morris_3d_engine/other/camera_controller.h"
    "engine/nine_morris_3d_engine/other/concurrent_loader.h"
    "engine/nine_morris_3d_engine/other/dependencies.h"
    "engine/nine_morris_3d_engine/other/encrypt.h"
    "engine/nine_morris_3d_engine/other/exit.h"
    "engine/nine_morris_3d_engine/other/logging.h"
    "engine/nine_morris_3d_engine/other/mesh.h"
    "engine/nine_morris_3d_engine/other/path.h"
    "engine/nine_morris_3d_engine/other/resource_manager.h"
    "engine/nine_morris_3d_engine/other/user_data.h"
)

add_library(${ENGINE} STATIC
    "engine/nine_morris_3d_engine/application/application_builder.cpp"
    "engine/nine_morris_3d_engine/application/application.cpp"
    "engine/nine_morris_3d_engine/application/capabilities.cpp"
    "engine/nine_morris_3d_engine/application/input.cpp"
    "engine/nine_morris_3d_engine/application/window.cpp"
    "engine/nine_morris_3d_engine/audio/openal/buffer.cpp"
    "engine/nine_morris_3d_engine/audio/openal/info_and_debug.cpp"
    "engine/nine_morris_3d_engine/audio/openal/listener.cpp"
    "engine/nine_morris_3d_engine/audio/openal/source.cpp"
    "engine/nine_morris_3d_engine/audio/context.cpp"
    "engine/nine_morris_3d_engine/audio/sound_data.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/buffer.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/framebuffer.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/info_and_debug.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/shader.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/texture.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/vertex_array.cpp"
    "engine/nine_morris_3d_engine/graphics/renderer/gui_renderer.cpp"
    "engine/nine_morris_3d_engine/graphics/renderer/renderer.cpp"
    "engine/nine_morris_3d_engine/graphics/buffer_layout.cpp"
    "engine/nine_morris_3d_engine/graphics/camera.cpp"
    "engine/nine_morris_3d_engine/graphics/font.cpp"
    "engine/nine_morris_3d_engine/graphics/identifier.cpp"
    "engine/nine_morris_3d_engine/graphics/imgui_context.cpp"
    "engine/nine_morris_3d_engine/graphics/material.cpp"
    "engine/nine_morris_3d_engine/graphics/texture_data.cpp"
    "engine/nine_morris_3d_engine/other/dependencies.cpp"
    "engine/nine_morris_3d_engine/other/encrypt.cpp"
    "engine/nine_morris_3d_engine/other/exit.cpp"
    "engine/nine_morris_3d_engine/other/logging.cpp"
    "engine/nine_morris_3d_engine/other/mesh.cpp"
    "engine/nine_morris_3d_engine/other/path.cpp"
    "engine/nine_morris_3d_engine/other/resource_manager.cpp"
    "engine/nine_morris_3d_engine/other/user_data.cpp"
    ${ENGINE_HEADERS}
)

add_library(${GLAD_STB} STATIC
    "dependencies/glad/src/glad.c"
    "dependencies/stb_image/stb_image.c"
    "dependencies/stb_truetype/stb_truetype.c"
    "dependencies/stb_image_write/stb_image_write.c"
    "dependencies/stb_vorbis/stb_vorbis.c"
)

add_library(${MY_IMGUI} STATIC
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
    "extern/imgui/backends/imgui_impl_glfw.cpp"
)

# Enable warnings
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(${ENGINE} PRIVATE -Wall -Wextra)
    target_compile_options(${GAME} PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${ENGINE} PRIVATE /W2)
    target_compile_options(${GAME} PRIVATE /W2)
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

# Use UTF-8 encoding on Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${ENGINE} PRIVATE /utf-8)
    target_compile_options(${GAME} PRIVATE /utf-8)
endif()

# Use C++17
target_compile_features(${ENGINE} PRIVATE cxx_std_17)
set_target_properties(${ENGINE} PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(${GAME} PRIVATE cxx_std_17)
set_target_properties(${GAME} PROPERTIES CXX_EXTENSIONS OFF)

# PCH
target_precompile_headers(${ENGINE} PRIVATE "engine/pch.h")
target_precompile_headers(${GAME} PRIVATE "src/pch.h")

add_subdirectory(extern/spdlog)
add_subdirectory(extern/glfw)
add_subdirectory(extern/assimp)
add_subdirectory(extern/cppblowfish)
add_subdirectory(extern/openal_soft)

add_dependencies(${ENGINE} glfw)  # This is necessary for some reason

# Add all include directories
target_include_directories(${ENGINE} PRIVATE
    "${CMAKE_SOURCE_DIR}/engine"
)
target_include_directories(${ENGINE} SYSTEM PUBLIC
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_truetype"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image_write"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_vorbis"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/assimp/include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/utfcpp/source"
    "${CMAKE_SOURCE_DIR}/extern/cppblowfish/include"
    "${CMAKE_SOURCE_DIR}/extern/entt/src"
    "${CMAKE_SOURCE_DIR}/extern/resmanager/src"
    "${CMAKE_SOURCE_DIR}/extern/openal_soft/include"
)

target_include_directories(${GAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)
target_include_directories(${GAME} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/engine"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/entt/src"
    "${CMAKE_SOURCE_DIR}/extern/resmanager/src"
)

target_include_directories(${GLAD_STB} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_truetype"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image_write"
)

target_include_directories(${MY_IMGUI} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
)

# Add all libraries
target_link_libraries(${ENGINE} PRIVATE
    spdlog
    assimp
    glfw
    cppblowfish
    OpenAL
    ${GLAD_STB}
    ${MY_IMGUI}
)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_link_libraries(${ENGINE} PRIVATE
        X11
    )
endif()

target_link_libraries(${GAME} PRIVATE
    ${ENGINE}
    ${MY_IMGUI}
    spdlog  # This avoids having to link engine libraries publicly
)

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET
            ${ENGINE} ${GAME} ${GLAD_STB} ${MY_IMGUI} spdlog assimp glfw cppblowfish OpenAL
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        set_property(TARGET
            ${ENGINE} ${GAME} ${GLAD_STB} ${MY_IMGUI} spdlog assimp glfw cppblowfish OpenAL
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded
        )
    endif()
endif()

# Add some defines
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building Nine Morris 3D in DEBUG mode")

    target_compile_definitions(${ENGINE} PRIVATE
        GAME_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES  # To easily enable or disable encrypted files
    )

    target_compile_definitions(${GAME} PRIVATE
        GAME_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES
    )
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message("Building Nine Morris 3D in RELEASE mode")

    target_compile_definitions(${ENGINE} PRIVATE
        NDEBUG
        ENTT_DISABLE_ASSERT
    )

    target_compile_definitions(${GAME} PRIVATE
        NDEBUG
        ENTT_DISABLE_ASSERT
    )
endif()

target_compile_definitions(${ENGINE} PRIVATE
    SPDLOG_ACTIVE_LEVEL=0
    _CRT_SECURE_NO_WARNINGS
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    ENTT_NO_ETO
    UTF_CPP_CPLUSPLUS=201703
    AL_LIBTYPE_STATIC
)

target_compile_definitions(${GAME} PRIVATE
    SPDLOG_ACTIVE_LEVEL=0
    _CRT_SECURE_NO_WARNINGS
    ENTT_NO_ETO
)
