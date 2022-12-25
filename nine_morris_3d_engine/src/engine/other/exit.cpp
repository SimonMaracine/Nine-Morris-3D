#include "engine/application/window.h"
#include "engine/audio/context.h"
#include "engine/other/exit.h"

namespace game_exit {
    void exit_critical() {
        // Gracefully clean up at least some memory
        destroy_openal_context();
        destroy_glfw_context();

        exit(1);
    }
}
