#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/audio/context.h"
#include "nine_morris_3d_engine/other/exit.h"

namespace game_exit {
    void exit_critical() {
        // Gracefully clean up at least some memory
        destroy_openal_context();
        destroy_glfw_context();

        exit(1);
    }
}
