#include "engine/application/panic.h"
#include "engine/application/window.h"
#include "engine/audio/context.h"

namespace panic {
    void panic() {
        // Gracefully clean up at least some resources
        OpenAlContext::destroy_openal_context();
        Window::destroy_glfw_context();

        exit(1);
    }
}
