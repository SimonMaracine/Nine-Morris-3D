cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(ENGINE NineMorris3D-Engine)
set(GAME Nine-Morris-3D)
set(GLAD_STB glad-stb)
set(MY_IMGUI my-imgui)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(BUILD_SHARED_LIBS OFF)
set(INJECT_DEBUG_POSTFIX OFF)  # This doesn't seem to work
set(CMAKE_DEBUG_POSTFIX "")

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(CPPBLOWFISH_BUILD_TESTS OFF)

project("Nine Morris 3D"
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
    "engine/nine_morris_3d_engine/application/events.h"
    "engine/nine_morris_3d_engine/application/capabilities.h"
    "engine/nine_morris_3d_engine/application/input.h"
    "engine/nine_morris_3d_engine/application/platform.h"
    "engine/nine_morris_3d_engine/application/scene.h"
    "engine/nine_morris_3d_engine/application/window.h"
    "engine/nine_morris_3d_engine/graphics/opengl/buffer.h"
    "engine/nine_morris_3d_engine/graphics/opengl/framebuffer.h"
    "engine/nine_morris_3d_engine/graphics/opengl/shader.h"
    "engine/nine_morris_3d_engine/graphics/opengl/texture.h"
    "engine/nine_morris_3d_engine/graphics/opengl/vertex_array.h"
    "engine/nine_morris_3d_engine/graphics/renderer/renderer.h"
    "engine/nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
    "engine/nine_morris_3d_engine/graphics/buffer_layout.h"
    "engine/nine_morris_3d_engine/graphics/camera.h"
    "engine/nine_morris_3d_engine/graphics/font.h"
    "engine/nine_morris_3d_engine/graphics/framebuffer_reader.h"
    "engine/nine_morris_3d_engine/graphics/identifier.h"
    "engine/nine_morris_3d_engine/graphics/light.h"
    "engine/nine_morris_3d_engine/graphics/material.h"
    "engine/nine_morris_3d_engine/graphics/post_processing_step.h"
    "engine/nine_morris_3d_engine/graphics/debug_opengl.h"
    "engine/nine_morris_3d_engine/other/assert.h"
    "engine/nine_morris_3d_engine/other/encrypt.h"
    "engine/nine_morris_3d_engine/other/concurrent_loader.h"
    "engine/nine_morris_3d_engine/other/logging.h"
    "engine/nine_morris_3d_engine/other/mesh.h"
    "engine/nine_morris_3d_engine/other/paths.h"
    "engine/nine_morris_3d_engine/other/texture_data.h"
    "engine/nine_morris_3d_engine/other/user_data.h"
    "engine/nine_morris_3d_engine/other/resource_manager.h"
    "engine/nine_morris_3d_engine/nine_morris_3d_engine.h"
)

add_library(${ENGINE} STATIC
    "engine/nine_morris_3d_engine/application/application_builder.cpp"
    "engine/nine_morris_3d_engine/application/application.cpp"
    "engine/nine_morris_3d_engine/application/capabilities.cpp"
    "engine/nine_morris_3d_engine/application/input.cpp"
    "engine/nine_morris_3d_engine/application/window.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/buffer.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/framebuffer.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/shader.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/texture.cpp"
    "engine/nine_morris_3d_engine/graphics/opengl/vertex_array.cpp"
    "engine/nine_morris_3d_engine/graphics/renderer/renderer.cpp"
    "engine/nine_morris_3d_engine/graphics/renderer/gui_renderer.cpp"
    "engine/nine_morris_3d_engine/graphics/buffer_layout.cpp"
    "engine/nine_morris_3d_engine/graphics/camera.cpp"
    "engine/nine_morris_3d_engine/graphics/font.cpp"
    "engine/nine_morris_3d_engine/graphics/identifier.cpp"
    "engine/nine_morris_3d_engine/graphics/material.cpp"
    "engine/nine_morris_3d_engine/graphics/debug_opengl.cpp"
    "engine/nine_morris_3d_engine/other/encrypt.cpp"
    "engine/nine_morris_3d_engine/other/logging.cpp"
    "engine/nine_morris_3d_engine/other/mesh.cpp"
    "engine/nine_morris_3d_engine/other/paths.cpp"
    "engine/nine_morris_3d_engine/other/texture_data.cpp"
    "engine/nine_morris_3d_engine/other/user_data.cpp"
    "engine/nine_morris_3d_engine/other/resource_manager.cpp"
    ${ENGINE_HEADERS}
)

add_library(${GLAD_STB} STATIC
    "dependencies/glad/src/glad.c"
    "dependencies/stb_image/stb_image.c"
    "dependencies/stb_truetype/stb_truetype.c"
    "dependencies/stb_image_write/stb_image_write.c"
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
    target_compile_options(${ENGINE} PRIVATE
        -Wall -Wextra
    )

    target_compile_options(${GAME} PRIVATE
        -Wall -Wextra
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${ENGINE} PRIVATE /w)  # FIXME
    string(REGEX REPLACE "/W[1-3]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    target_compile_options(${GAME} PRIVATE /w)  # FIXME
    string(REGEX REPLACE "/W[1-3]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

# Fix encoding on Windows
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

add_dependencies(${ENGINE} glfw)  # This is necessary for some reason

# Add all include directories
target_include_directories(${ENGINE} PRIVATE
    "${CMAKE_SOURCE_DIR}/engine"
)
target_include_directories(${ENGINE} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_truetype"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image_write"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/assimp/include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/utfcpp/source"
    "${CMAKE_SOURCE_DIR}/extern/cppblowfish/include"
    "${CMAKE_SOURCE_DIR}/extern/entt/src"
    "${CMAKE_SOURCE_DIR}/extern/resmanager/src"
)

target_include_directories(${GAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)
target_include_directories(${GAME} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/engine"
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_truetype"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image_write"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/utfcpp/source"
    "${CMAKE_SOURCE_DIR}/extern/cppblowfish/include"
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

# Add all link directories
if(CMAKE_BUILD_TYPE MATCHES Debug)
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        target_link_directories(${ENGINE} PRIVATE
            "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/build/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/build/extern/glfw/src"
        )

        target_link_directories(${GAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        target_link_directories(${ENGINE} PRIVATE
            "${CMAKE_SOURCE_DIR}/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/extern/glfw/src"
        )

        target_link_directories(${GAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/extern/spdlog"
        )
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        target_link_directories(${ENGINE} PRIVATE
            "${CMAKE_SOURCE_DIR}/build-release/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/build-release/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/build-release/extern/glfw/src"
        )

        target_link_directories(${GAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/build-release/extern/spdlog"
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        target_link_directories(${ENGINE} PRIVATE
            "${CMAKE_SOURCE_DIR}/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/extern/glfw/src"
        )

        target_link_directories(${GAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/extern/spdlog"
        )
    endif()
endif()

# Change working directory for Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set_target_properties(
        ${GAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

# Add all libraries
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_link_libraries(${ENGINE} PRIVATE
        spdlog assimp glfw cppblowfish X11
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_link_libraries(${ENGINE} PRIVATE
        spdlog assimp glfw cppblowfish
    )
endif()

target_link_libraries(${ENGINE} PRIVATE
    "${CMAKE_DL_LIBS}"
    ${GLAD_STB}
    ${MY_IMGUI}
)

target_link_libraries(${GAME} PRIVATE
    ${ENGINE}
    ${MY_IMGUI}
    spdlog
)

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET
            ${ENGINE} ${GAME} ${GLAD_STB} ${MY_IMGUI} spdlog assimp glfw cppblowfish
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        set_property(TARGET
            ${ENGINE} ${GAME} ${GLAD_STB} ${MY_IMGUI} spdlog assimp glfw cppblowfish
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded
        )
    endif()
endif()

# Add some defines
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building in DEBUG mode")

    target_compile_definitions(${ENGINE} PRIVATE
        SPDLOG_ACTIVE_LEVEL=0  # PRINT_GPU_RAM_ALLOCATED
    )

    target_compile_definitions(${GAME} PRIVATE
        SPDLOG_ACTIVE_LEVEL=0  # PRINT_GPU_RAM_ALLOCATED
    )
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message("Building in RELEASE mode")

    target_compile_definitions(${ENGINE} PRIVATE
        SPDLOG_ACTIVE_LEVEL=0 NDEBUG ENTT_DISABLE_ASSERT
    )

    target_compile_definitions(${GAME} PRIVATE
        SPDLOG_ACTIVE_LEVEL=0 NDEBUG ENTT_DISABLE_ASSERT
    )
endif()

target_compile_definitions(${ENGINE} PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    ENTT_NO_ETO
    UTF_CPP_CPLUSPLUS=201703
)

target_compile_definitions(${GAME} PRIVATE
    ENTT_NO_ETO
)
