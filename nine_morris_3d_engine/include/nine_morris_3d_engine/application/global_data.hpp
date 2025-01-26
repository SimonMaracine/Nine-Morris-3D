#pragma once

namespace sm {
    // Must be inherited from the user's global struct of data
    struct GlobalData {
        virtual ~GlobalData() = default;
    };
}
