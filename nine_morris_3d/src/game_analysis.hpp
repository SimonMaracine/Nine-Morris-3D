#pragma once

#include <vector>
#include <memory>
#include <cstddef>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "engines/engine.hpp"
#include "clock.hpp"

class GameScene;

class GameAnalysis {
public:
    GameAnalysis(std::size_t game_index, std::shared_ptr<UciLikeEngine> engine);
    ~GameAnalysis();

    GameAnalysis(const GameAnalysis&) = default;
    GameAnalysis& operator=(const GameAnalysis&) = default;
    GameAnalysis(GameAnalysis&&) = default;
    GameAnalysis& operator=(GameAnalysis&&) = default;

    void set_engine_callback();
    void update_evaluation_bar();
    void evaluation_bar_window(const sm::Ctx& ctx, const GameScene& game_scene);

    std::size_t get_game_index() const { return m_game_index; }
    const std::string& get_pv() const { return m_pv; }

    bool thinking {false};
    Clock::Time time_white {};
    Clock::Time time_black {};
    std::size_t ply {0};
private:
    void information_callback(const UciLikeEngine::Info& info);

    enum class ScoreType {
        Eval,
        Win
    };

    std::size_t m_game_index {};  // Index into saved games
    std::shared_ptr<UciLikeEngine> m_engine;

    std::string m_pv;
    int m_score {};
    ScoreType m_score_type {};

    int m_old_score {};  // Used to detect changes to score for linear interpolation
    float m_white_fill_current {};  // What is diplayed on the screen
    float m_white_fill_real {};  // What is the actual value
    float m_interpolation {};  // Normalized value that signifies how close current is to real
};
