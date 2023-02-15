#pragma once

class Application;

namespace event {
    struct Event;
}

namespace dummy {
    struct UserFunc {
        void operator()(Application*) {}
    };

    struct ProcFunc {
        void operator()() {}
    };

    struct OnEventFunc {
        void operator()(event::Event&) {}
    };

    struct UserData {};

    inline std::any empty_user_data() {
        return std::make_any<UserData>();
    }
}
