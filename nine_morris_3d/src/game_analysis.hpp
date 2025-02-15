#pragma once

#include <cstddef>

class GameAnalysis {
public:
    explicit GameAnalysis(std::size_t index)
        : m_index(index) {}

    std::size_t get_index() const { return m_index; }

    unsigned int clock_white {};
    unsigned int clock_black {};
    std::size_t ply {0};
private:
    std::size_t m_index {};
};
