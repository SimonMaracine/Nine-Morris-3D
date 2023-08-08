#pragma once

namespace sm {
    class Ctx;

    namespace dummy {
        struct UserFunc {
            void operator()(Ctx*) {}
        };

        struct ProcFunc {
            void operator()() {}
        };

        struct UserData {};
    }
}
