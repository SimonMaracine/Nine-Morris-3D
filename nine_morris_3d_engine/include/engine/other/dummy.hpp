#pragma once

namespace sm {  // TODO get rid of this
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
