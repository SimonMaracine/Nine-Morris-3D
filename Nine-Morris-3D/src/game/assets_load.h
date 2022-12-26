#pragma once

#include <engine/engine_other.h>

namespace assets_load {
    using AllStartLoader = ConcurrentLoader<bool, int, bool, int>;
    using SkyboxLoader = ConcurrentLoader<int, int>;
    using BoardPaintTextureLoader = ConcurrentLoader<int, bool>;

    void all_start(AllStartLoader& loader, bool normal_mapping, int texture_quality, bool labeled_board, int skybox);
    void skybox(SkyboxLoader& loader, int texture_quality, int skybox);
    void board_paint_texture(BoardPaintTextureLoader& loader, int texture_quality, bool labeled_board);
}
