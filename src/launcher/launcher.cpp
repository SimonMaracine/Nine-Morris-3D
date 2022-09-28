#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/assets.h"
#include "launcher/launcher.h"

using namespace paths;
using namespace assets;

namespace launcher {
    void start(Application* app) {
        std::array<std::unique_ptr<TextureData>, 5> icons = {
            std::make_unique<TextureData>(path_for_assets(ICON_512), false),
            std::make_unique<TextureData>(path_for_assets(ICON_256), false),
            std::make_unique<TextureData>(path_for_assets(ICON_128), false),
            std::make_unique<TextureData>(path_for_assets(ICON_64), false),
            std::make_unique<TextureData>(path_for_assets(ICON_32), false)
        };

        app->window->set_icons<5>(icons);
    }
}
