#include <string>
#include <utility>
#include <iostream>
#include <cassert>
#include <stdio.h>

#include <glad/glad.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <assimp/version.h>
#include <glm/glm.hpp>
#include <spdlog/version.h>
#include <nlohmann/json.hpp>
#include <cereal/version.hpp>

#include "other/logging.h"

constexpr GLenum parameters[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_LOD_BIAS,
    GL_MAX_VIEWPORT_DIMS,
    GL_STEREO
};

const char* names[] = {
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
    "GL_MAX_DRAW_BUFFERS",
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_TEXTURE_SIZE",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_LOD_BIAS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO"
};

#ifndef NDEBUG
unsigned long long approximately_bytes_allocated_gpu = 0;
bool stop_counting_bytes_allocated_gpu = false;
#endif

namespace debug_opengl {
#if 0
    static const std::string parse_version(int version) {
        int major, minor, patch;
        int numbers[5];

        for (int i = 4; i >= 0; i--) {
            int digit = version % 10;
            numbers[i] = digit;
            version /= 10;
        }

        major = numbers[0];
        minor = numbers[2];
        patch = numbers[4];

        char str[50];
        sprintf(str, "%d.%d.%d", major, minor, patch);

        return std::string(str);
    }
#endif

#ifndef NDEBUG
    static void error_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* userParam) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                spdlog::critical("Debug Message:");
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                switch (id) {
                    case 14:
                    case 13:
                    case 10:
                    case 9:
                    case 7:
                        return;  // Ignore these warnings
                }
                spdlog::warn("Debug Message:");
                break;
            case GL_DEBUG_SEVERITY_LOW:
                spdlog::warn("Debug Message:");
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                assert(false);
        }

        switch (source) {
            case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
        }
        std::cout << std::endl;

        switch (type) {
            case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
        }
        std::cout << std::endl;

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                spdlog::critical("(ID: {}) {}", id, message);
                std::exit(1);
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_LOW:
                spdlog::warn("(ID: {}) {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                assert(false);
        }
    }
#endif

    void maybe_init_debugging() {
#ifndef NDEBUG
        glDebugMessageCallback(error_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                              0, nullptr, GL_FALSE);
        spdlog::info("OpenGL message callback is set");
#endif
    }

    const std::string get_info() {
        std::string output;

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** OpenGL Version And Driver Information ***\n");

        {
            char line[100];  // 100 should be enough
            sprintf(line, "OpenGL version: %s\n", glGetString(GL_VERSION));
            output.append(line);
            sprintf(line, "GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
            output.append(line);
            sprintf(line, "Vendor: %s\n", glGetString(GL_VENDOR));
            output.append(line);
            sprintf(line, "Renderer: %s\n", glGetString(GL_RENDERER));
            output.append(line);
        }

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** OpenGL Context Parameters ***\n");

        for (int i = 0; i <= 9; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[100];
            sprintf(line, "%s %i\n", names[i], result);
            output.append(line);
        }
        {
            GLint result[2];
            glGetIntegerv(parameters[10], result);

            char line[100];
            sprintf(line, "%s %i %i\n", names[10], result[0], result[1]);
            output.append(line);
        }
        {
            GLboolean result;
            glGetBooleanv(parameters[11], &result);

            char line[100];
            sprintf(line, "%s %u\n", names[11], (unsigned int) result);
            output.append(line);
        }

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** Dependencies Versions ***\n");

#if defined(__GNUG__)
        {
            char line[100];
            sprintf(line, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
            output.append(line);
        }
#elif defined(_MSC_VER)
        {
            char line[100];
            sprintf(line, "MSVC version: %d\n", _MSC_VER);
            output.append(line);
        }
#else
    #error "GCC or MSVC must be used (for now)"
#endif
        {
            char line[100];
            sprintf(line, "GLFW version: %s\n", glfwGetVersionString());
            output.append(line);
        }
        {
            char line[100];
            sprintf(line, "Dear ImGui version: %s\n", ImGui::GetVersion());
            output.append(line);
        }
        {
            char line[100];
            sprintf(line, "Assimp version: %d.%d.%d\n", aiGetVersionMajor(),
                    aiGetVersionMinor(), aiGetVersionPatch());
            output.append(line);
        }
        {
            char line[100];
            sprintf(line, "GLM version: %d.%d.%d\n", GLM_VERSION_MAJOR, GLM_VERSION_MINOR,
                    GLM_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[100];
            sprintf(line, "spdlog version: %d.%d.%d\n", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR,
                    SPDLOG_VER_PATCH);
            output.append(line);
        }
        {
            char line[100];
            sprintf(line, "json version: %d.%d.%d\n", NLOHMANN_JSON_VERSION_MAJOR,
                    NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[100];
            sprintf(line, "cereal version: %d.%d.%d\n", CEREAL_VERSION_MAJOR,
                    CEREAL_VERSION_MINOR, CEREAL_VERSION_PATCH);
            output.append(line);
        }

        return output;
    }

    const std::pair<int, int> get_version() {
        int major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        return std::make_pair(major, minor);
    }
}
