#include <string>
#include <utility>
#include <cstddef>
#include <cstdio>

#include <glad/glad.h>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/capabilities.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/graphics/opengl/info_and_debug.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    static const GLenum parameters[] {
        GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        GL_MAX_CUBE_MAP_TEXTURE_SIZE,
        GL_MAX_DRAW_BUFFERS,
        GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        GL_MAX_TEXTURE_IMAGE_UNITS,
        GL_MAX_TEXTURE_SIZE,
        GL_MAX_RENDERBUFFER_SIZE,
        GL_MAX_VERTEX_ATTRIBS,
        GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_TEXTURE_LOD_BIAS,
        GL_MAX_UNIFORM_BUFFER_BINDINGS,
        GL_MAX_UNIFORM_BLOCK_SIZE,
        GL_MAX_VERTEX_UNIFORM_BLOCKS,
        GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
        GL_MAX_SAMPLES,
        GL_MAX_DEPTH_TEXTURE_SAMPLES,
        GL_MAX_COLOR_TEXTURE_SAMPLES,
        GL_MAX_INTEGER_SAMPLES,
        GL_MAX_VIEWPORT_DIMS
    };

    static const char* names[] {
        "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
        "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
        "GL_MAX_DRAW_BUFFERS",
        "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
        "GL_MAX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_TEXTURE_SIZE",
        "GL_MAX_RENDERBUFFER_SIZE",
        "GL_MAX_VERTEX_ATTRIBS",
        "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
        "GL_MAX_TEXTURE_LOD_BIAS",
        "GL_MAX_UNIFORM_BUFFER_BINDINGS",
        "GL_MAX_UNIFORM_BLOCK_SIZE",
        "GL_MAX_VERTEX_UNIFORM_BLOCKS",
        "GL_MAX_FRAGMENT_UNIFORM_BLOCKS",
        "GL_MAX_SAMPLES",
        "GL_MAX_DEPTH_TEXTURE_SAMPLES",
        "GL_MAX_COLOR_TEXTURE_SAMPLES",
        "GL_MAX_INTEGER_SAMPLES",
        "GL_MAX_VIEWPORT_DIMS"
    };

    static constexpr std::size_t BUFFER_LENGTH {128};

#ifdef SM_BUILD_DEBUG
    static void error_callback(
        GLenum,
        GLenum,
        GLuint id,
        GLenum severity,
        GLsizei,
        const GLchar* message,
        const void*
    ) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                LOG_CRITICAL("({}) OpenGL: {}", id, message);
                throw OpenGlError;

                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_LOW:
                LOG_WARNING("({}) OpenGL: {}", id, message);

                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                SM_ASSERT(false, "This should have been disabled");

                break;
        }
    }
#endif

    void GlInfoDebug::maybe_initialize_debugging() {
#ifdef SM_BUILD_DEBUG
        glDebugMessageCallback(error_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(
            GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DEBUG_SEVERITY_NOTIFICATION,
            0,
            nullptr,
            GL_FALSE
        );

        LOG_INFO("OpenGL message callback is set");
#endif
    }

    std::string GlInfoDebug::get_info() {
        std::string output;
        output.reserve(1024);

        output.append("\n*** OpenGL Version And Driver ***\n");

        {
            static constexpr std::size_t LENGTH {256};  // 256 should be enough

            char line[LENGTH];
            std::snprintf(line, LENGTH, "OpenGL version: %s\n", glGetString(GL_VERSION));
            output.append(line);
            std::snprintf(line, LENGTH, "GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
            output.append(line);
            std::snprintf(line, LENGTH, "Vendor: %s\n", glGetString(GL_VENDOR));
            output.append(line);
            std::snprintf(line, LENGTH, "Renderer: %s\n", glGetString(GL_RENDERER));
            output.append(line);
        }

        output.append("\n*** OpenGL Context Parameters ***\n");

        std::size_t parameter_index {18};

        for (std::size_t i {0}; i <= parameter_index; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[BUFFER_LENGTH];
            std::snprintf(line, BUFFER_LENGTH, "%s %i\n", names[i], result);
            output.append(line);
        }
        {
            GLint result[2];
            glGetIntegerv(parameters[++parameter_index], result);

            char line[BUFFER_LENGTH];
            std::snprintf(line, BUFFER_LENGTH, "%s %i %i\n", names[parameter_index], result[0], result[1]);
            output.append(line);
        }

        output.append("\n*** OpenGL Extensions ***\n");

        char line[BUFFER_LENGTH];
        std::snprintf(
            line,
            BUFFER_LENGTH,
            "GL_EXT_texture_filter_anisotropic max samples: %d\n",
            max_anisotropic_filtering_supported()
        );
        output.append(line);

        return output;
    }

    std::pair<int, int> GlInfoDebug::get_version_number() {
        int major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        return std::make_pair(major, minor);
    }

    const unsigned char* GlInfoDebug::get_opengl_version() {
        return glGetString(GL_VERSION);
    }

    const unsigned char* GlInfoDebug::get_glsl_version() {
        return glGetString(GL_SHADING_LANGUAGE_VERSION);
    }

    const unsigned char* GlInfoDebug::get_vendor() {
        return glGetString(GL_VENDOR);
    }

    const unsigned char* GlInfoDebug::get_renderer() {
        return glGetString(GL_RENDERER);
    }
}
