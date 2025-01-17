#pragma once

#include <string>
#include <vector>

class MoveList {
public:
    void push(const std::string& string);
    void clear();
    void skip_first(bool skip);
    void update_window() const;
private:
    std::vector<std::string> m_moves;
    bool m_skip_first {false};
};
