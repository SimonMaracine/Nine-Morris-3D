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
#include <entt/entt.hpp>

#include "other/logging.h"

constexpr GLenum parameters[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_FLOATS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
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
    "GL_MAX_VARYING_FLOATS",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO"
};

namespace debug_opengl {
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

    static void error_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                               GLsizei length, const GLchar* message,
                               const void* userParam) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                spdlog::critical("(ID: {}) {}", id, message);
                std::exit(1);
            case GL_DEBUG_SEVERITY_MEDIUM:
                switch (id) {
                    case 10:
                    case 7: return;  // Ignore these warnings
                }
                spdlog::warn("(ID: {}) {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                spdlog::info("(ID: {}) {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                assert(false);
        }
    }

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

        for (int i = 0; i < 10; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[50];  // 50 must be enough
            sprintf(line, "%s %i\n", names[i], result);
            output.append(line);
        }
        {
            GLint result[2];
            glGetIntegerv(parameters[10], result);

            char line[50];
            sprintf(line, "%s %i %i\n", names[10], result[0], result[1]);
            output.append(line);
        }
        {
            GLboolean result;
            glGetBooleanv(parameters[11], &result);

            char line[50];
            sprintf(line, "%s %u\n", names[11], (unsigned int) result);
            output.append(line);
        }

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** Dependencies Versions ***\n");

        {
            char line[50];
            sprintf(line, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
            output.append(line);
        }
        {
            char line[50];
            sprintf(line, "GLFW version: %s\n", glfwGetVersionString());
            output.append(line);
        }
        {
            char line[50];
            sprintf(line, "Dear ImGui version: %s\n", ImGui::GetVersion());
            output.append(line);
        }
        {
            char line[50];
            sprintf(line, "Assimp version: %d.%d.%d\n", aiGetVersionMajor(),
                    aiGetVersionMinor(), aiGetVersionPatch());
            output.append(line);
        }
        {
            char line[50];
            sprintf(line, "GLM version: %d.%d.%d\n", GLM_VERSION_MAJOR, GLM_VERSION_MINOR,
                    GLM_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[50];
            const char* version = parse_version(SPDLOG_VERSION).c_str();
            sprintf(line, "spdlog version: %s\n", version);
            output.append(line);
        }
        {
            char line[50];
            sprintf(line, "EnTT version: %d.%d.%d\n", ENTT_VERSION_MAJOR,
                    ENTT_VERSION_MINOR, ENTT_VERSION_PATCH);
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
