#include "game_analysis.hpp"

#include <numeric>

GameAnalysis::GameAnalysis(std::size_t game_index, std::shared_ptr<UciLikeEngine> engine)
    : m_game_index(game_index), m_engine(engine) {}

GameAnalysis::~GameAnalysis() {
    if (m_engine) {
        m_engine->set_info_callback({});
    }
}

void GameAnalysis::set_engine_callback() {
    m_engine->set_info_callback([this](const UciLikeEngine::Info& info) { information_callback(info); });
}

void GameAnalysis::information_callback(const UciLikeEngine::Info& info) {
    if (info.score) {
        switch (info.score->index()) {
            case 0:
                m_score = "eval " + std::to_string(std::get<0>(*info.score).value);
                break;
            case 1:
                m_score = "win " + std::to_string(std::get<1>(*info.score).value);
                break;
        }
    }

    if (info.pv) {
        if (info.pv->empty()) {
            m_pv.clear();
        } else {
            m_pv = std::accumulate(++info.pv->cbegin(), info.pv->cend(), *info.pv->cbegin(), [](std::string r, const std::string& move) {
                return std::move(r) + " " + move;
            });
        }
    }
}
