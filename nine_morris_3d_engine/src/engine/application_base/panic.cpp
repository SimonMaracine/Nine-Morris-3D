#include "engine/application_base/window.h"
#include "engine/audio/context.h"
#include "engine/application_base/panic.h"

namespace panic {
    void panic() {
        // Gracefully clean up at least some resources
        destroy_openal_context();
        destroy_glfw_context();

        exit(1);
    }
}
