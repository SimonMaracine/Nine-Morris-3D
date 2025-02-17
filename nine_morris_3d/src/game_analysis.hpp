#pragma once

#include <vector>
#include <memory>
#include <cstddef>

#include "engines/engine.hpp"
#include "clock.hpp"

class GameAnalysis {
public:
    GameAnalysis(std::size_t game_index, std::shared_ptr<UciLikeEngine> engine);
    ~GameAnalysis();

    GameAnalysis(const GameAnalysis&) = default;
    GameAnalysis& operator=(const GameAnalysis&) = default;
    GameAnalysis(GameAnalysis&&) = default;
    GameAnalysis& operator=(GameAnalysis&&) = default;

    void set_engine_callback();

    std::size_t get_game_index() const { return m_game_index; }
    const std::string& get_score() const { return m_score; }
    const std::string& get_pv() const { return m_pv; }

    bool thinking {false};
    Clock::Time time_white {};
    Clock::Time time_black {};
    std::size_t ply {0};
private:
    void information_callback(const UciLikeEngine::Info& info);

    std::size_t m_game_index {};  // Index into saved games
    std::shared_ptr<UciLikeEngine> m_engine;

    std::string m_score;
    std::string m_pv;
};
