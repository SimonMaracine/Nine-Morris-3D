#pragma once

#include <chrono>

// Object used to slow down the main loop to a certain target rate
class Clock {
public:
    // Start the clock
    Clock();

    // Sleep, if the time elapsed is lower than the target time
    void sleep(std::chrono::high_resolution_clock::duration target) const;
private:
    std::chrono::high_resolution_clock::time_point m_start;
};
