#include <engine/public/application_base.h>
#include <engine/public/other.h>

#include "game/assets.h"
#include "launcher/launcher.h"

namespace launcher {
    void start(sm::Ctx* ctx) {
        using namespace sm::file_system;
        using namespace assets;

        const auto icons = {
            std::make_unique<sm::TextureData>(path_for_assets(ICON_512), false),
            std::make_unique<sm::TextureData>(path_for_assets(ICON_256), false),
            std::make_unique<sm::TextureData>(path_for_assets(ICON_128), false),
            std::make_unique<sm::TextureData>(path_for_assets(ICON_64), false),
            std::make_unique<sm::TextureData>(path_for_assets(ICON_32), false)
        };

        ctx->window->set_icons(icons);
    }
}
