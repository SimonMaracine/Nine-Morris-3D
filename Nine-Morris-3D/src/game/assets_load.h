#pragma once

#include <engine/public/other.h>

namespace assets_load {
    struct AllStartLoader : ConcurrentLoader<AllStartLoader, bool, int, bool, int> {
        AllStartLoader(const ConcurrentLoader::Callback& callback_function)
            : ConcurrentLoader(callback_function) {}

        void load(bool normal_mapping, int texture_quality, bool labeled_board, int skybox);
    };

    struct SkyboxLoader : ConcurrentLoader<SkyboxLoader, int, int> {
        SkyboxLoader(const ConcurrentLoader::Callback& callback_function)
            : ConcurrentLoader(callback_function) {}

        void load(int texture_quality, int skybox);
    };

    struct BoardPaintTextureLoader : ConcurrentLoader<BoardPaintTextureLoader, int, bool> {
        BoardPaintTextureLoader(const ConcurrentLoader::Callback& callback_function)
            : ConcurrentLoader(callback_function) {}

        void load(int texture_quality, bool labeled_board);
    };
}
