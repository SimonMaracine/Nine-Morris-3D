#pragma once

#include <string>
#include <vector>

class MovesList {
public:
    void push(const std::string& string);
    void clear();
    void skip_first(bool skip);
    void moves_window() const;

    const std::vector<std::string>& get_moves() const { return m_moves; }
private:
    std::vector<std::string> m_moves;
    bool m_skip_first {false};
};
