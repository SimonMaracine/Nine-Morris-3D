#include "nine_morris_3d_engine/other/exit.h"

#include <GLFW/glfw3.h>

namespace game_exit {
    void exit_critical() {
        glfwTerminate();  // Gracefully clean up at least some memory
        exit(1);
    }
}
