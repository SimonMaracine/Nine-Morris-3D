#include <cstdlib>

#include "engine/application_base/panic.hpp"
#include "engine/application_base/window.hpp"
#include "engine/audio/context.hpp"

namespace sm {
    void panic() {
        // Gracefully clean up at least some resources
        OpenAlContext::destroy_openal_context();
        Window::destroy_glfw_context();

        std::exit(1);
    }
}
