#pragma once

namespace sm {
    // Base class for the global data
    // Must be inherited from the user's global struct of data
    struct GlobalData {
        virtual ~GlobalData() = default;  // A single virtual call at shutdown plus 8 more bytes of memory (oh, no)
    };
}
