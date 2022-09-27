#include <glad/glad.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <assimp/version.h>
#include <glm/glm.hpp>
#include <spdlog/version.h>
#include <entt/entt.hpp>
#include <cereal/version.hpp>
#include <cppblowfish/cppblowfish.h>
#include <resmanager/resmanager.h>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/application/extensions.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"

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
    GL_MAX_UNIFORM_BUFFER_BINDINGS,
    GL_MAX_DEPTH_TEXTURE_SAMPLES,
    GL_MAX_COLOR_TEXTURE_SAMPLES,
    GL_MAX_INTEGER_SAMPLES,
    GL_MAX_VIEWPORT_DIMS,
};

constexpr const char* names[] = {
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
    "GL_MAX_UNIFORM_BUFFER_BINDINGS",
    "GL_MAX_DEPTH_TEXTURE_SAMPLES",
    "GL_MAX_COLOR_TEXTURE_SAMPLES",
    "GL_MAX_INTEGER_SAMPLES",
    "GL_MAX_VIEWPORT_DIMS",
};

#ifdef PLATFORM_GAME_DEBUG
    GpuMemoryCounter _gpu_mem_counter;
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

#ifdef PLATFORM_GAME_DEBUG
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
                REL_CRITICAL("(ID: {}) {}", id, message);
                exit(1);
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_LOW:
                REL_WARN("(ID: {}) {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                ASSERT(false, "This should have been disabled");
        }
    }
#endif

    void maybe_initialize_debugging() {
#ifdef PLATFORM_GAME_DEBUG
        glDebugMessageCallback(error_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                0, nullptr, GL_FALSE);
        DEB_INFO("OpenGL message callback is set");
#endif
    }

    std::string get_info() {
        std::string output;

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** OpenGL Version And Driver Information ***\n");

        {
            char line[128];  // 128 should be enough
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

        size_t parameter_index = 13;

        for (size_t i = 0; i <= parameter_index; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[128];
            sprintf(line, "%s %i\n", names[i], result);
            output.append(line);
        }
        {
            GLint result[2];
            glGetIntegerv(parameters[++parameter_index], result);

            char line[128];
            sprintf(line, "%s %i %i\n", names[parameter_index], result[0], result[1]);
            output.append(line);
        }

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** OpenGL Extensions ***\n");

        char line[128];
        sprintf(line, "GL_EXT_texture_filter_anisotropic is supported: %s\n",
                extensions::extension_supported(extensions::AnisotropicFiltering) ? "true" : "false");
        output.append(line);

        //////////////////////////////////////////////////////////////////////////////////
        output.append("\n*** Dependencies And Versions ***\n");

#if defined(PLATFORM_GAME_LINUX)
        {
            char line[128];
            sprintf(line, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
            output.append(line);
        }
#elif defined(PLATFORM_GAME_WINDOWS)
        {
            char line[128];
            sprintf(line, "MSVC version: %d\n", _MSC_VER);
            output.append(line);
        }
#endif
        {
            char line[128];
            sprintf(line, "GLFW version: %s\n", glfwGetVersionString());
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "Dear ImGui version: %s\n", ImGui::GetVersion());
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "EnTT version: %d.%d.%d\n", ENTT_VERSION_MAJOR, ENTT_VERSION_MINOR,
                    ENTT_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "Assimp version: %d.%d.%d\n", aiGetVersionMajor(),
                    aiGetVersionMinor(), aiGetVersionPatch());
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "GLM version: %d.%d.%d\n", GLM_VERSION_MAJOR, GLM_VERSION_MINOR,
                    GLM_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "spdlog version: %d.%d.%d\n", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR,
                    SPDLOG_VER_PATCH);
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "cereal version: %d.%d.%d\n", CEREAL_VERSION_MAJOR,
                    CEREAL_VERSION_MINOR, CEREAL_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "cppblowfish version: %d.%d.%d\n", CPPBLOWFISH_VERSION_MAJOR,
                    CPPBLOWFISH_VERSION_MINOR, CPPBLOWFISH_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "resmanager version: %d.%d.%d\n", RESMANAGER_VERSION_MAJOR,
                    RESMANAGER_VERSION_MINOR, RESMANAGER_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "glad\n");
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "stb_image\n");
            output.append(line);
        }
        {   
            char line[128];
            sprintf(line, "stb_image_write\n");
            output.append(line);
        }
        {   
            char line[128];
            sprintf(line, "stb_truetype\n");
            output.append(line);
        }
        {
            char line[128];
            sprintf(line, "utfcpp\n");
            output.append(line);
        }

        return output;
    }

    std::pair<int, int> get_version_numbers() {
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
