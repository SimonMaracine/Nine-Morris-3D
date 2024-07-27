#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <glad/glad.h>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/graphics/opengl/capabilities.hpp"

namespace sm {
    static const unsigned int parameters[] {
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

    void opengl_debug::initialize() noexcept {
        glDebugMessageCallback(
            [](
                unsigned int,
                unsigned int,
                [[maybe_unused]] unsigned int id,
                unsigned int severity,
                int,
                [[maybe_unused]] const char* message,
                const void*
            ) {
                switch (severity) {
                    case GL_DEBUG_SEVERITY_HIGH:
                        LOG_CRITICAL("({}) OpenGL: {}", id, message);
                        std::abort();
                        break;
                    case GL_DEBUG_SEVERITY_MEDIUM:
                    case GL_DEBUG_SEVERITY_LOW:
                        LOG_WARNING("({}) OpenGL: {}", id, message);
                        break;
                    case GL_DEBUG_SEVERITY_NOTIFICATION:
                        assert(false);
                        break;
                }
            },
            nullptr
        );

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

        LOG_INFO("Set OpenGL message callback");
    }

    std::string opengl_debug::get_information() {
        char buffer[BUFFER_LENGTH] {};
        std::string result;

        result += "*** OpenGL Version And Driver ***\n";

        {
            static constexpr std::size_t BUFFER_LENGTH {256};  // Should be enough

            char buffer[BUFFER_LENGTH] {};

            std::snprintf(buffer, BUFFER_LENGTH, "OpenGL version: %s\n", glGetString(GL_VERSION));
            result += buffer;
            std::snprintf(buffer, BUFFER_LENGTH, "GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
            result += buffer;
            std::snprintf(buffer, BUFFER_LENGTH, "Renderer: %s\n", glGetString(GL_RENDERER));
            result += buffer;
            std::snprintf(buffer, BUFFER_LENGTH, "Vendor: %s\n", glGetString(GL_VENDOR));
            result += buffer;
        }

        result += "*** OpenGL Context Parameters ***\n";

        std::size_t parameter_index {18};

        for (std::size_t i {0}; i <= parameter_index; i++) {
            int value {};
            glGetIntegerv(parameters[i], &value);

            std::snprintf(buffer, BUFFER_LENGTH, "%s %i\n", names[i], value);
            result += buffer;
        }
        {
            int value[2] {};
            glGetIntegerv(parameters[++parameter_index], value);

            std::snprintf(buffer, BUFFER_LENGTH, "%s %i %i\n", names[parameter_index], value[0], value[1]);
            result += buffer;
        }

        result += "*** OpenGL Extensions ***\n";

        std::snprintf(
            buffer,
            BUFFER_LENGTH,
            "GL_EXT_texture_filter_anisotropic max samples: %d\n",
            capabilities::max_anisotropic_filtering_supported()
        );
        result += buffer;

        return result;
    }

    std::pair<int, int> opengl_debug::get_version_number() noexcept {
        int major {};
        int minor {};
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        return std::make_pair(major, minor);
    }

    const unsigned char* opengl_debug::get_opengl_version() noexcept {
        return glGetString(GL_VERSION);
    }

    const unsigned char* opengl_debug::get_glsl_version() noexcept {
        return glGetString(GL_SHADING_LANGUAGE_VERSION);
    }

    const unsigned char* opengl_debug::get_vendor() noexcept {
        return glGetString(GL_VENDOR);
    }

    const unsigned char* opengl_debug::get_renderer() noexcept {
        return glGetString(GL_RENDERER);
    }
}
