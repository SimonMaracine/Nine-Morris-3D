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
#include "nine_morris_3d_engine/other/dependencies.h"

static constexpr size_t BUFFER_LENGTH = 128;

namespace dependencies {
    std::string get_info() {
        std::string output;

        output.append("\n*** Compiler ***\n");

#if defined(NM3D_PLATFORM_LINUX)
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
            output.append(line);
        }
#elif defined(NM3D_PLATFORM_WINDOWS)
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "MSVC version: %d\n", _MSC_VER);
            output.append(line);
        }
#endif

        output.append("\n*** Dependencies ***\n");

        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "GLFW version: %s\n", glfwGetVersionString());
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "Dear ImGui version: %s\n", ImGui::GetVersion());
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "EnTT version: %d.%d.%d\n", ENTT_VERSION_MAJOR,
                ENTT_VERSION_MINOR, ENTT_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "Assimp version: %d.%d.%d\n", aiGetVersionMajor(),
                aiGetVersionMinor(), aiGetVersionPatch());
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "GLM version: %d.%d.%d\n", GLM_VERSION_MAJOR,
                GLM_VERSION_MINOR, GLM_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "spdlog version: %d.%d.%d\n", SPDLOG_VER_MAJOR,
                SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "cereal version: %d.%d.%d\n", CEREAL_VERSION_MAJOR,
                CEREAL_VERSION_MINOR, CEREAL_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "cppblowfish version: %d.%d.%d\n", CPPBLOWFISH_VERSION_MAJOR,
                CPPBLOWFISH_VERSION_MINOR, CPPBLOWFISH_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "resmanager version: %d.%d.%d\n", RESMANAGER_VERSION_MAJOR,
                RESMANAGER_VERSION_MINOR, RESMANAGER_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "OpenAL Soft\n");
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "glad\n");
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "stb_image\n");
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "stb_image_write\n");
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "stb_truetype\n");
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "stb_vorbis\n");
            output.append(line);
        }
        {
            char line[BUFFER_LENGTH];
            snprintf(line, BUFFER_LENGTH, "utfcpp\n");
            output.append(line);
        }

        return output;
    }
}
