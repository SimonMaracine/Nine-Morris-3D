#pragma once

namespace sm {
    // void panic();

    enum RuntimeError {
        ErrorInitialization,
        ErrorResourceLoading,
        ErrorOpenAl,
        ErrorOpenGl,
        ErrorOther
    };
}
