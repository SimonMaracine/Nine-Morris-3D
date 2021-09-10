cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(EXECUTABLE_NAME "Nine-Morris-3D")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(ASSIMP_BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_INSTALL_PDB OFF)  # TODO Should check this on MSVC

project("Nine Morris 3D"
    VERSION 0.1.0
    LANGUAGES C CXX)

add_executable(${EXECUTABLE_NAME}
    "src/main.cpp"
    "src/application/application.cpp"
    "src/application/input.cpp"
    "src/application/window.cpp"
    "src/application/scenes/game/game_scene.cpp"
    "src/application/scenes/game/game_layer.cpp"
    "src/application/scenes/game/imgui_layer.cpp"
    "src/application/scenes/game/gui_layer.cpp"
    "src/application/scenes/loading/loading_scene.cpp"
    "src/application/scenes/loading/loading_layer.cpp"
    "src/opengl/debug_opengl.cpp"
    "src/opengl/renderer/vertex_array.cpp"
    "src/opengl/renderer/buffer.cpp"
    "src/opengl/renderer/buffer_layout.cpp"
    "src/opengl/renderer/shader.cpp"
    "src/opengl/renderer/renderer.cpp"
    "src/opengl/renderer/texture.cpp"
    "src/opengl/renderer/framebuffer.cpp"
    "src/other/logging.cpp"
    "src/other/model.cpp"
    "src/other/texture_data.cpp"
    "src/other/loader.cpp"
    "src/other/options.cpp"
    "src/other/save_load.cpp"
    "src/other/asset_manager.cpp"
    "src/ecs_and_game/systems.cpp"
    "src/ecs_and_game/game.cpp"
    "dependencies/Glad/src/glad.c"
    "dependencies/stb_image/stb_image.c"
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
    "extern/imgui/backends/imgui_impl_glfw.cpp")

# Enable warnings
target_compile_options(${EXECUTABLE_NAME} PUBLIC
    -Wall -Wextra
    -Wno-unused-parameter -Wno-missing-field-initializers -Wno-type-limits
    -Wno-parentheses)

# Use C++17
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_17)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

add_subdirectory(extern/spdlog)
add_subdirectory(extern/Assimp)
add_subdirectory(extern/entt)
add_subdirectory(extern/glm)
add_subdirectory(extern/glfw)
add_subdirectory(extern/json)

# Add all include directories
target_include_directories(${EXECUTABLE_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${EXECUTABLE_NAME} SYSTEM PUBLIC
    "${CMAKE_SOURCE_DIR}/dependencies/Glad/include"
    "${CMAKE_SOURCE_DIR}/dependencies/stb_image"
    "${CMAKE_SOURCE_DIR}/dependencies/cereal/include"
    "${CMAKE_SOURCE_DIR}/extern/spdlog/include"
    "${CMAKE_SOURCE_DIR}/extern/Assimp/include"
    "${CMAKE_SOURCE_DIR}/extern/entt/single_include"
    "${CMAKE_SOURCE_DIR}/extern/glm"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include"
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/json/single_include")

# Add all link directories
target_link_directories(${EXECUTABLE_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/build/extern/spdlog"
    "${CMAKE_SOURCE_DIR}/build/extern/Assmip/bin"
    "${CMAKE_SOURCE_DIR}/build/extern/glfw/src")

# Add all libraries
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    spdlog assimp glfw3 X11)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC "${CMAKE_DL_LIBS}")

# Add some defines
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building in DEBUG mode")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC
        SPDLOG_ACTIVE_LEVEL=0)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message("Building in RELEASE mode")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC
        SPDLOG_ACTIVE_LEVEL=6 NDEBUG ENTT_DISABLE_ASSERT)
endif()

target_compile_definitions(${EXECUTABLE_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
