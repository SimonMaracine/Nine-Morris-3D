#pragma once

#include <cstddef>

#include "clock.hpp"

class GameAnalysis {
public:
    explicit GameAnalysis(std::size_t index)
        : m_index(index) {}

    std::size_t get_index() const { return m_index; }

    Clock::Time time_white {};
    Clock::Time time_black {};
    std::size_t ply {0};
private:
    std::size_t m_index {};
};
