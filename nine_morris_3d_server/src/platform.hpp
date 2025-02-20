#pragma once

// Used to differentiate between testing and distribution builds
#ifdef NM3D_DISTRIBUTION_MODE  // Defined in CMake
    #define SM_BUILD_DISTRIBUTION
#endif
