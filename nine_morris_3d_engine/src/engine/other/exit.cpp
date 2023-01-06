#include "engine/application/window.h"
#include "engine/audio/context.h"
#include "engine/other/exit.h"

namespace application_exit {
    void panic() {
        // Gracefully clean up at least some resources
        destroy_openal_context();
        destroy_glfw_context();

        exit(1);
    }
}
