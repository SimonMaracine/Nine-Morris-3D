#include "nine_morris_3d_engine/other/dependencies.hpp"

#include <cstdio>

#include <imgui.h>
#include <SDL3/SDL.h>
#include <assimp/version.h>
#include <glm/glm.hpp>
#include <spdlog/version.h>
#include <cereal/version.hpp>
#include <resmanager/resmanager.hpp>
#include <entt/config/version.h>
#include <boost/version.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"

namespace sm::dependencies {
    std::string get_information() {
        char buffer[128] {};
        std::string result;

        result += "*** Compiler ***\n";

#if defined(SM_PLATFORM_LINUX)
        std::snprintf(buffer, sizeof(buffer), "GCC: %d.%d\n", __GNUC__, __GNUC_MINOR__);
        result += buffer;
#elif defined(SM_PLATFORM_WINDOWS)
        std::snprintf(buffer, sizeof(buffer), "MSVC: %d\n", _MSC_VER);
        result += buffer;
#endif

        result += "*** Engine Dependencies ***\n";

        std::snprintf(buffer, sizeof(buffer), "SDL: %d\n", SDL_GetVersion());
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "Dear ImGui: %s\n", ImGui::GetVersion());
        result += buffer;

        std::snprintf(
            buffer,
            sizeof(buffer),
            "EnTT: %d.%d.%d\n",
            ENTT_VERSION_MAJOR,
            ENTT_VERSION_MINOR,
            ENTT_VERSION_PATCH
        );
        result += buffer;

        std::snprintf(
            buffer,
            sizeof(buffer),
            "Assimp: %u.%u.%u\n",
            aiGetVersionMajor(),
            aiGetVersionMinor(),
            aiGetVersionPatch()
        );
        result += buffer;

        std::snprintf(
            buffer,
            sizeof(buffer),
            "GLM: %d.%d.%d\n",
            GLM_VERSION_MAJOR,
            GLM_VERSION_MINOR,
            GLM_VERSION_PATCH
        );
        result += buffer;

        std::snprintf(
            buffer,
            sizeof(buffer),
            "spdlog: %d.%d.%d\n",
            SPDLOG_VER_MAJOR,
            SPDLOG_VER_MINOR,
            SPDLOG_VER_PATCH
        );
        result += buffer;

        std::snprintf(
            buffer,
            sizeof(buffer),
            "cereal: %d.%d.%d\n",
            CEREAL_VERSION_MAJOR,
            CEREAL_VERSION_MINOR,
            CEREAL_VERSION_PATCH
        );
        result += buffer;

        std::snprintf(
            buffer,
            sizeof(buffer),
            "resmanager: %u.%u.%u\n",
            resmanager::VERSION_MAJOR,
            resmanager::VERSION_MINOR,
            resmanager::VERSION_PATCH
        );
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "boost: %s\n", BOOST_LIB_VERSION);
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "OpenAL Soft\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "Glad\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "stb_image\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "stb_image_write\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "stb_truetype\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "stb_vorbis\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "stb_image_resize2\n");
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "utfcpp\n");
        result += buffer;

        return result;
    }
}
