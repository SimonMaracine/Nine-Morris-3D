#pragma once

#include <utility>
#include <cstddef>

struct GLFWmonitor;

namespace sm {
    namespace internal {
        class Window;
    }

    class Monitors {
    public:
        std::pair<int, int> get_resolution(std::size_t index) const;
        std::pair<float, float> get_content_scale(std::size_t index) const;
        const char* get_name(std::size_t index) const;
        std::size_t get_count() const;
    private:
        GLFWmonitor** monitors {};
        std::size_t count {};

        friend class internal::Window;
    };
}
