#include "engine/application_base/panic.h"
#include "engine/application_base/window.h"
#include "engine/audio/context.h"

namespace panic {
    void panic() {
        // Gracefully clean up at least some resources
        OpenAlContext::destroy_openal_context();
        Window::destroy_glfw_context();

        exit(1);
    }
}
