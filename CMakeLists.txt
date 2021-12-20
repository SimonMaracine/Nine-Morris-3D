cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(EXECUTABLE_NAME Nine-Morris-3D)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT VSCODE)
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

project("Nine Morris 3D"
    VERSION 0.1.0
    LANGUAGES C CXX)

add_executable(${EXECUTABLE_NAME}
    "src/main.cpp"
    "src/application/application.cpp"
    "src/application/input.cpp"
    "src/application/window.cpp"
    "src/nine_morris_3d/scenes/game/game_scene.cpp"
    "src/nine_morris_3d/layers/game/game_layer.cpp"
    "src/nine_morris_3d/layers/game/imgui_layer.cpp"
    "src/nine_morris_3d/layers/game/gui_layer.cpp"
    "src/nine_morris_3d/scenes/loading/loading_scene.cpp"
    "src/nine_morris_3d/layers/loading/loading_layer.cpp"
    "src/nine_morris_3d/options.cpp"
    "src/nine_morris_3d/save_load.cpp"
    "src/nine_morris_3d/assets_load.cpp"
    "src/nine_morris_3d/piece.cpp"
    "src/nine_morris_3d/node.cpp"
    "src/nine_morris_3d/board.cpp"
    "src/nine_morris_3d/hoverable.cpp"
    "src/nine_morris_3d/turn_indicator.cpp"
    "src/opengl/debug_opengl.cpp"
    "src/opengl/renderer/vertex_array.cpp"
    "src/opengl/renderer/buffer.cpp"
    "src/opengl/renderer/buffer_layout.cpp"
    "src/opengl/renderer/shader.cpp"
    "src/opengl/renderer/renderer.cpp"
    "src/opengl/renderer/texture.cpp"
    "src/opengl/renderer/framebuffer.cpp"
    "src/opengl/renderer/camera.cpp"
    "src/other/logging.cpp"
    "src/other/model.cpp"
    "src/other/texture_data.cpp"
    "src/other/assets.cpp"
    "src/other/user_data.cpp"
    "dependencies/glad/src/glad.c"
    "dependencies/stb_image/stb_image.c"
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
    "extern/imgui/backends/imgui_impl_glfw.cpp")

# Enable warnings
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(${EXECUTABLE_NAME} PUBLIC
        -Wall -Wextra
        -Wno-unused-parameter -Wno-missing-field-initializers -Wno-type-limits
        -Wno-parentheses)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    # target_compile_options(${EXECUTABLE_NAME} PUBLIC -Wall)
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

# Fix encoding on Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${EXECUTABLE_NAME} PUBLIC /utf-8)
endif()

# Use C++17
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_17)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

add_subdirectory(extern/spdlog)
add_subdirectory(extern/glm)
add_subdirectory(extern/glfw)
add_subdirectory(extern/json)
add_subdirectory(extern/assimp)

add_dependencies(${EXECUTABLE_NAME} glfw)  # This is necessary for some reason

# Add all include directories
target_include_directories(${EXECUTABLE_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${EXECUTABLE_NAME} SYSTEM PUBLIC
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/assimp/include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/json/single_include")

# Add all link directories
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_directories(${EXECUTABLE_NAME} PUBLIC
            "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/build/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/build/extern/glfw/src")
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        target_link_directories(${EXECUTABLE_NAME} PUBLIC
            "${CMAKE_SOURCE_DIR}/build-release/extern/spdlog"
            "${CMAKE_SOURCE_DIR}/build-release/extern/assimp/bin"
            "${CMAKE_SOURCE_DIR}/build-release/extern/glfw/src")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(VSCODE MATCHES ON)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            target_link_directories(${EXECUTABLE_NAME} PUBLIC
                "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/build/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/build/extern/glfw/src")
        elseif(CMAKE_BUILD_TYPE MATCHES Release)
            target_link_directories(${EXECUTABLE_NAME} PUBLIC
                "${CMAKE_SOURCE_DIR}/build-release/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/build-release/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/build-release/extern/glfw/src")
        endif()
    elseif(VSCODE MATCHES OFF)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            target_link_directories(${EXECUTABLE_NAME} PUBLIC
                "${CMAKE_SOURCE_DIR}/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/extern/glfw/src")
        elseif(CMAKE_BUILD_TYPE MATCHES Release)
            target_link_directories(${EXECUTABLE_NAME} PUBLIC
                "${CMAKE_SOURCE_DIR}/extern/spdlog"
                "${CMAKE_SOURCE_DIR}/extern/assimp/bin"
                "${CMAKE_SOURCE_DIR}/extern/glfw/src")
        endif()
    endif()
endif()

# Add all libraries
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC
        spdlog assimp glfw X11)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC
        spdlog assimp glfw)
endif()

target_link_libraries(${EXECUTABLE_NAME} PUBLIC "${CMAKE_DL_LIBS}")

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET spdlog PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET spdlog PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
    endif()
endif()

# Add some defines
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building in DEBUG mode")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC
        SPDLOG_ACTIVE_LEVEL=0)  # PRINT_GPU_RAM_ALLOCATED
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message("Building in RELEASE mode")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC
        SPDLOG_ACTIVE_LEVEL=6 NDEBUG)
endif()

target_compile_definitions(${EXECUTABLE_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# See what is setting up on Windows
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(VSCODE MATCHES ON)
        message("Windows build NOT using Visual Studio")
    elseif(VSCODE MATCHES OFF)
        message("Windows build using Visual Studio")
    endif()
endif()
