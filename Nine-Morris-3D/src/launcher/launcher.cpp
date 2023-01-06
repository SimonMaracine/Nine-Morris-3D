#include <engine/engine_application.h>
#include <engine/engine_other.h>

#include "game/assets.h"
#include "launcher/launcher.h"

namespace launcher {
    void start(Application* app) {
        using namespace file_system;
        using namespace assets;

        const auto icons = {
            std::make_unique<TextureData>(path_for_assets(ICON_512), false),
            std::make_unique<TextureData>(path_for_assets(ICON_256), false),
            std::make_unique<TextureData>(path_for_assets(ICON_128), false),
            std::make_unique<TextureData>(path_for_assets(ICON_64), false),
            std::make_unique<TextureData>(path_for_assets(ICON_32), false)
        };

        app->window->set_icons(icons);
    }
}
