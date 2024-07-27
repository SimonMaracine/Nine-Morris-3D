#include "nine_morris_3d_engine/other/dependencies.hpp"

#include <cstdio>
#include <cstddef>

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <assimp/version.h>
#include <glm/glm.hpp>
#include <spdlog/version.h>
#include <cereal/version.hpp>
#include <resmanager/resmanager.hpp>

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <entt/entt.hpp>

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

#include "nine_morris_3d_engine/application/platform.hpp"

namespace sm {
    static constexpr std::size_t BUFFER_LENGTH {128};

    std::string dependencies::get_information() {
        char buffer[BUFFER_LENGTH] {};
        std::string result;

        result += "*** Compiler ***\n";

        {
#if defined(SM_PLATFORM_LINUX)
            std::snprintf(buffer, BUFFER_LENGTH, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
            result += buffer;
#elif defined(SM_PLATFORM_WINDOWS)
            std::snprintf(buffer, BUFFER_LENGTH, "MSVC version: %d\n", _MSC_VER);
            result += buffer;
#endif
        }

        result += "*** Dependencies ***\n";

        {
            std::snprintf(buffer, BUFFER_LENGTH, "GLFW version: %s\n", glfwGetVersionString());
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "Dear ImGui version: %s\n", ImGui::GetVersion());
            result += buffer;
        }
        {
            std::snprintf(
                buffer,
                BUFFER_LENGTH,
                "EnTT version: %d.%d.%d\n",
                ENTT_VERSION_MAJOR,
                ENTT_VERSION_MINOR,
                ENTT_VERSION_PATCH
            );
            result += buffer;
        }
        {
            std::snprintf(
                buffer,
                BUFFER_LENGTH,
                "Assimp version: %d.%d.%d\n",
                aiGetVersionMajor(),
                aiGetVersionMinor(),
                aiGetVersionPatch()
            );
            result += buffer;
        }
        {
            std::snprintf(
                buffer,
                BUFFER_LENGTH,
                "GLM version: %d.%d.%d\n",
                GLM_VERSION_MAJOR,
                GLM_VERSION_MINOR,
                GLM_VERSION_PATCH
            );
            result += buffer;
        }
        {
            std::snprintf(
                buffer,
                BUFFER_LENGTH,
                "spdlog version: %d.%d.%d\n",
                SPDLOG_VER_MAJOR,
                SPDLOG_VER_MINOR,
                SPDLOG_VER_PATCH
            );
            result += buffer;
        }
        {
            std::snprintf(
                buffer,
                BUFFER_LENGTH,
                "cereal version: %d.%d.%d\n",
                CEREAL_VERSION_MAJOR,
                CEREAL_VERSION_MINOR,
                CEREAL_VERSION_PATCH
            );
            result += buffer;
        }
        {
            std::snprintf(
                buffer,
                BUFFER_LENGTH,
                "resmanager version: %d.%d.%d\n",
                resmanager::VERSION_MAJOR,
                resmanager::VERSION_MINOR,
                resmanager::VERSION_PATCH
            );
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "OpenAL Soft\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "glad\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "stb_image\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "stb_image_write\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "stb_truetype\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "stb_vorbis\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "stb_image_resize2\n");
            result += buffer;
        }
        {
            std::snprintf(buffer, BUFFER_LENGTH, "utfcpp\n");
            result += buffer;
        }

        return result;
    }
}
