#pragma once

class Application;

namespace dummy {
    struct UserFunc {
        void operator()(Application*) {}
    };

    struct ProcFunc {
        void operator()() {}
    };

    struct UserData {};

    inline std::any empty_user_data() {
        return std::make_any<UserData>();
    }
}
