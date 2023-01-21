#include <glad/glad.h>

#include "engine/application/platform.h"
#include "engine/application/capabilities.h"
#include "engine/graphics/opengl/info_and_debug.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/exit.h"

static constexpr GLenum parameters[] = {
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
    GL_MAX_SAMPLES,
    GL_MAX_DEPTH_TEXTURE_SAMPLES,
    GL_MAX_COLOR_TEXTURE_SAMPLES,
    GL_MAX_INTEGER_SAMPLES,
    GL_MAX_VIEWPORT_DIMS
};

static constexpr const char* names[] = {
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
    "GL_MAX_SAMPLES",
    "GL_MAX_DEPTH_TEXTURE_SAMPLES",
    "GL_MAX_COLOR_TEXTURE_SAMPLES",
    "GL_MAX_INTEGER_SAMPLES",
    "GL_MAX_VIEWPORT_DIMS"
};

static constexpr size_t BUFFER_LENGTH = 128;

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

#ifdef NM3D_PLATFORM_DEBUG
    static void error_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei, const GLchar* message, const void*) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                std::cout << "OpenGL Debug Message:" << std::endl;
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_LOW:
                std::cout << "OpenGL Debug Message:" << std::endl;
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                ASSERT(false, "This should have been disabled");
                break;
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
                REL_CRITICAL("({}) {}", id, message);
                application_exit::panic();
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_LOW:
                REL_WARNING("({}) {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                ASSERT(false, "This should have been disabled");
                break;
        }
    }
#endif

namespace gl {
    void maybe_initialize_debugging() {
#ifdef NM3D_PLATFORM_DEBUG
        glDebugMessageCallback(error_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE
        );

        DEB_INFO("OpenGL message callback is set");
#endif
    }

    std::string get_info() {
        std::string output;

        output.append("\n*** OpenGL Version And Driver ***\n");

        {
            constexpr size_t LENGTH = 256;  // 256 should be enough

            char line[LENGTH];
            snprintf(line, LENGTH, "OpenGL version: %s\n", glGetString(GL_VERSION));
            output.append(line);
            snprintf(line, LENGTH, "GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
            output.append(line);
            snprintf(line, LENGTH, "Vendor: %s\n", glGetString(GL_VENDOR));
            output.append(line);
            snprintf(line, LENGTH, "Renderer: %s\n", glGetString(GL_RENDERER));
            output.append(line);
        }

        output.append("\n*** OpenGL Context Parameters ***\n");

        size_t parameter_index = 15;

        for (size_t i = 0; i <= parameter_index; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "%s %i\n", names[i], result);
            output.append(line);
        }
        {
            GLint result[2];
            glGetIntegerv(parameters[++parameter_index], result);

            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "%s %i %i\n", names[parameter_index], result[0], result[1]);
            output.append(line);
        }

        output.append("\n*** OpenGL Extensions ***\n");

        char line[BUFFER_LENGTH];
        snprintf(line, BUFFER_LENGTH, "GL_EXT_texture_filter_anisotropic max samples: %d\n",
                capabilities::max_anisotropic_filtering_supported());
        output.append(line);

        return output;
    }

    std::pair<int, int> get_version_number() {
        int major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        return std::make_pair(major, minor);
    }

    const unsigned char* get_opengl_version() {
        return glGetString(GL_VERSION);
    }

    const unsigned char* get_glsl_version() {
        return glGetString(GL_SHADING_LANGUAGE_VERSION);
    }

    const unsigned char* get_vendor() {
        return glGetString(GL_VENDOR);
    }

    const unsigned char* get_renderer() {
        return glGetString(GL_RENDERER);
    }
}
