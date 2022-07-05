cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(EXECUTABLE_NAME Nine-Morris-3D)
set(GLAD_STB glad-stb)
set(MY_IMGUI my-imgui)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT DEFINED VSCODE)
    set(VSCODE OFF)
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

set(HEADERS_FOR_WINDOWS
    "src/pch.h"
    "src/application/application.h"
    "src/application/application_data.h"
    "src/application/events.h"
    "src/application/input.h"
    "src/application/layer.h"
    "src/application/platform.h"
    "src/application/scene.h"
    "src/application/window.h"
    "src/application/extensions.h"
    "src/graphics/debug_opengl.h"
    "src/graphics/renderer/renderer.h"
    "src/graphics/renderer/gui_renderer.h"
    "src/graphics/renderer/buffer_layout.h"
    "src/graphics/renderer/camera.h"
    "src/graphics/renderer/font.h"
    "src/graphics/renderer/framebuffer_reader.h"
    "src/graphics/renderer/hoverable.h"
    "src/graphics/renderer/light.h"
    "src/graphics/renderer/material.h"
    "src/graphics/renderer/opengl/vertex_array.h"
    "src/graphics/renderer/opengl/buffer.h"
    "src/graphics/renderer/opengl/shader.h"
    "src/graphics/renderer/opengl/texture.h"
    "src/graphics/renderer/opengl/framebuffer.h"
    "src/nine_morris_3d/nine_morris_3d.h"
    "src/nine_morris_3d/layers/game/game_layer.h"
    "src/nine_morris_3d/layers/game/imgui_layer.h"
    "src/nine_morris_3d/layers/game/gui_layer.h"
    "src/nine_morris_3d/layers/loading/loading_layer.h"
    "src/nine_morris_3d/options.h"
    "src/nine_morris_3d/save_load.h"
    "src/nine_morris_3d/assets_data.h"
    "src/nine_morris_3d/board.h"
    "src/nine_morris_3d/node.h"
    "src/nine_morris_3d/piece.h"
    "src/nine_morris_3d/timer.h"
    "src/nine_morris_3d/assets.h"
    "src/nine_morris_3d/keyboard_controls.h"
    "src/nine_morris_3d/constants.h"
    "src/other/paths.h"
    "src/other/assert.h"
    "src/other/loader.h"
    "src/other/logging.h"
    "src/other/mesh.h"
    "src/other/texture_data.h"
    "src/other/user_data.h"
    "src/other/encryption.h"
)

# Add WIN32 option for Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Release)
        set(NO_CONSOLE WIN32)
    endif()
endif()

add_executable(${EXECUTABLE_NAME} ${NO_CONSOLE}
    "src/main.cpp"
    "src/application/application.cpp"
    "src/application/input.cpp"
    "src/application/window.cpp"
    "src/application/extensions.cpp"
    "src/graphics/debug_opengl.cpp"
    "src/graphics/renderer/renderer.cpp"
    "src/graphics/renderer/gui_renderer.cpp"
    "src/graphics/renderer/buffer_layout.cpp"
    "src/graphics/renderer/camera.cpp"
    "src/graphics/renderer/font.cpp"
    "src/graphics/renderer/hoverable.cpp"
    "src/graphics/renderer/material.cpp"
    "src/graphics/renderer/opengl/vertex_array.cpp"
    "src/graphics/renderer/opengl/buffer.cpp"
    "src/graphics/renderer/opengl/shader.cpp"
    "src/graphics/renderer/opengl/texture.cpp"
    "src/graphics/renderer/opengl/framebuffer.cpp"
    "src/nine_morris_3d/nine_morris_3d.cpp"
    "src/nine_morris_3d/layers/game/game_layer.cpp"
    "src/nine_morris_3d/layers/game/imgui_layer.cpp"
    "src/nine_morris_3d/layers/game/gui_layer.cpp"
    "src/nine_morris_3d/layers/loading/loading_layer.cpp"
    "src/nine_morris_3d/options.cpp"
    "src/nine_morris_3d/save_load.cpp"
    "src/nine_morris_3d/assets_data.cpp"
    "src/nine_morris_3d/board.cpp"
    "src/nine_morris_3d/timer.cpp"
    "src/nine_morris_3d/assets.cpp"
    "src/nine_morris_3d/keyboard_controls.cpp"
    "src/other/paths.cpp"
    "src/other/logging.cpp"
    "src/other/mesh.cpp"
    "src/other/texture_data.cpp"
    "src/other/user_data.cpp"
    "src/other/encryption.cpp"
    ${HEADERS_FOR_WINDOWS}
)

# On Windows tell linker to use main instead of WinMain
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        /entry:mainCRTStartup
    )
endif()

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
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        -Wall -Wextra
        -Wno-unused-parameter -Wno-parentheses -Wno-type-limits
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /w)  # FIXME
    string(REGEX REPLACE "/W[1-3]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

# Fix encoding on Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /utf-8)
endif()

# Use C++17
target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_17)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# PCH
target_precompile_headers(${EXECUTABLE_NAME} PRIVATE "src/pch.h")

add_subdirectory(extern/spdlog)
add_subdirectory(extern/glm)
add_subdirectory(extern/glfw)
add_subdirectory(extern/json)
add_subdirectory(extern/assimp)
add_subdirectory(extern/utfcpp)
add_subdirectory(extern/cppblowfish)

add_dependencies(${EXECUTABLE_NAME} glfw)  # This is necessary for some reason

# Add all include directories
target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_truetype"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image_write"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/assimp/include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/json/single_include"
    "${CMAKE_SOURCE_DIR}/extern/utfcpp/source"
    "${CMAKE_SOURCE_DIR}/extern/cppblowfish/include"
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
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_directories(${EXECUTABLE_NAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/build/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/build/extern/glfw/src"
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        target_link_directories(${EXECUTABLE_NAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/build-release/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/build-release/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/build-release/extern/glfw/src"
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(VSCODE MATCHES ON)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            target_link_directories(${EXECUTABLE_NAME} PRIVATE
                "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/build/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/build/extern/glfw/src"
            )
        elseif(CMAKE_BUILD_TYPE MATCHES Release)
            target_link_directories(${EXECUTABLE_NAME} PRIVATE
                "${CMAKE_SOURCE_DIR}/build-release/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/build-release/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/build-release/extern/glfw/src"
            )
        endif()
    elseif(VSCODE MATCHES OFF)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            target_link_directories(${EXECUTABLE_NAME} PRIVATE
                "${CMAKE_SOURCE_DIR}/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/extern/glfw/src"
            )
        elseif(CMAKE_BUILD_TYPE MATCHES Release)
            target_link_directories(${EXECUTABLE_NAME} PRIVATE
                "${CMAKE_SOURCE_DIR}/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/extern/glfw/src"
            )
        endif()
    endif()
endif()

# Add all libraries
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        spdlog assimp glfw cppblowfish X11
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        spdlog assimp glfw cppblowfish
    )
endif()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE "${CMAKE_DL_LIBS}")
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${GLAD_STB})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${MY_IMGUI})

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET spdlog PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET cppblowfish PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET ${GLAD_STB} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET ${MY_IMGUI} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET spdlog PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET cppblowfish PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET ${GLAD_STB} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET ${MY_IMGUI} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
    endif()
endif()

# Add some defines
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building in DEBUG mode")
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
        SPDLOG_ACTIVE_LEVEL=0  # PRINT_GPU_RAM_ALLOCATED
    )
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message("Building in RELEASE mode")
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
        SPDLOG_ACTIVE_LEVEL=0 NDEBUG
    )
endif()

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# See what is setting up on Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(VSCODE MATCHES ON)
        message("Windows build NOT using Visual Studio")
    elseif(VSCODE MATCHES OFF)
        message("Windows build using Visual Studio")
    endif()
endif()
