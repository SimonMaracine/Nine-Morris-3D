#include <string>
#include <cstdio>
#include <cstddef>

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <assimp/version.h>
#include <glm/glm.hpp>
#include <spdlog/version.h>
#include <entt/entt.hpp>
#include <cereal/version.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/application_base/platform.hpp"
#include "engine/other/dependencies.hpp"

namespace sm {
    static constexpr std::size_t BUFFER_LEN {128};

    std::string Dependencies::get_information() {
        std::string output;
        output.reserve(512);

        output.append("\n*** Compiler ***\n");

#if defined(SM_PLATFORM_LINUX)
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
            output.append(line);
        }
#elif defined(SM_PLATFORM_WINDOWS)
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "MSVC version: %d\n", _MSC_VER);
            output.append(line);
        }
#endif

        output.append("\n*** Dependencies ***\n");

        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "GLFW version: %s\n", glfwGetVersionString());
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "Dear ImGui version: %s\n", ImGui::GetVersion());
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "EnTT version: %d.%d.%d\n", ENTT_VERSION_MAJOR,
                ENTT_VERSION_MINOR, ENTT_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "Assimp version: %d.%d.%d\n", aiGetVersionMajor(),
                aiGetVersionMinor(), aiGetVersionPatch());
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "GLM version: %d.%d.%d\n", GLM_VERSION_MAJOR,
                GLM_VERSION_MINOR, GLM_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "spdlog version: %d.%d.%d\n", SPDLOG_VER_MAJOR,
                SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "cereal version: %d.%d.%d\n", CEREAL_VERSION_MAJOR,
                CEREAL_VERSION_MINOR, CEREAL_VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "resmanager version: %d.%d.%d\n", resmanager::VERSION_MAJOR,
                resmanager::VERSION_MINOR, resmanager::VERSION_PATCH);
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "OpenAL Soft\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "glad\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "stb_image\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "stb_image_write\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "stb_truetype\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "stb_vorbis\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "stb_image_resize2\n");
            output.append(line);
        }
        {
            char line[BUFFER_LEN];
            std::snprintf(line, BUFFER_LEN, "utfcpp\n");
            output.append(line);
        }

        return output;
    }
}
