#pragma once

#include <utility>
#include <cstddef>

struct GLFWmonitor;

namespace sm {
    namespace internal {
        class Window;
    }

    // Query display monitors
    // Returned by the window
    class Monitors {
    public:
        std::pair<int, int> get_resolution(std::size_t index) const;
        std::pair<float, float> get_content_scale(std::size_t index) const noexcept;
        const char* get_name(std::size_t index) const;
        std::size_t get_count() const noexcept;
    private:
        GLFWmonitor** m_monitors {};
        std::size_t m_count {};

        friend class internal::Window;
    };
}
