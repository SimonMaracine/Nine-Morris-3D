#pragma once

#include "engine/application/window.h"

namespace imgui_context {
    void initialize(const std::unique_ptr<Window>& window);
    void uninitialize();

    void begin_frame();
    void end_frame();
}
