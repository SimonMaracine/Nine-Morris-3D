#include <stdio.h>
#include <string>
#include <utility>
#include <iostream>

#include "glad/glad.h"

#include "logging.h"

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
    static void error_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                               GLsizei length, const GLchar* message,
                               const void* userParam) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                std::cerr << "[SEVERITY HIGH] " << message << std::endl;
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                std::cerr << "[SEVERITY MEDIUM] " << message << std::endl;
                break;
            case GL_DEBUG_SEVERITY_LOW:
                std::cerr << "[SEVERITY LOW] " << message << std::endl;
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                std::cerr << "[SEVERITY NOTIFICATION] " << message << std::endl;
                break;
        }
    }

    void maybe_init_debugging() {
#if !defined(NDEBUG)
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

        output.append("\n*** OpenGL Version And Driver Information ***\n");

        char line[100];  // 100 should be enough
        sprintf(line, "OpenGL version: %s\n", glGetString(GL_VERSION));
        output.append(line);
        sprintf(line, "GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        output.append(line);
        sprintf(line, "Vendor: %s\n", glGetString(GL_VENDOR));
        output.append(line);
        sprintf(line, "Renderer: %s\n", glGetString(GL_RENDERER));
        output.append(line);

        output.append("\n*** OpenGL Context Parameters ***\n");

        for (int i = 0; i < 10; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[50];  // 50 must be enough
            sprintf(line, "%s %i\n", names[i], result);
            output.append(line);
        }

        GLint result[2];
        glGetIntegerv(parameters[10], result);

        char line2[50];  // 50 must be enough
        sprintf(line2, "%s %i %i\n", names[10], result[0], result[1]);
        output.append(line2);
    
        GLboolean result2;
        glGetBooleanv(parameters[11], &result2);

        char line3[50];  // 50 must be enough
        sprintf(line3, "%s %u\n", names[11], (unsigned int) result2);
        output.append(line3);

        return output;
    }

    const std::pair<int, int> get_version() {
        int major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

        return std::make_pair(major, minor);
    }
}
