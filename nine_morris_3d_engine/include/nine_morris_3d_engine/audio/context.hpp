#pragma once

#include "nine_morris_3d_engine/audio/openal/listener.hpp"

struct ALCdevice;
struct ALCcontext;

namespace sm {
    namespace internal {
        class OpenAlContext {
        public:
            explicit OpenAlContext(bool create = true);
            ~OpenAlContext();

            OpenAlContext(const OpenAlContext&) = delete;
            OpenAlContext& operator=(const OpenAlContext&) = delete;
            OpenAlContext(OpenAlContext&&) = delete;
            OpenAlContext& operator=(OpenAlContext&&) = delete;

            AlListener& get_listener() { return listener; }
        private:
            bool create {};
            AlListener listener;

            ALCdevice* device {nullptr};
            ALCcontext* context {nullptr};
        };
    }
}
