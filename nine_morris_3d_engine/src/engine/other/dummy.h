#pragma once

class Ctx;

namespace event {
    struct Event;
}

namespace dummy {
    struct UserFunc {
        void operator()(Ctx*) {}
    };

    struct ProcFunc {
        void operator()() {}
    };

    struct OnEventFunc {
        void operator()(event::Event&) {}
    };

    struct UserData {};

    // inline std::any empty_user_data() {  // TODO remove
    //     return std::make_any<UserData>();
    // }
}
