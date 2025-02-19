#include "game_analysis.hpp"

#include <numeric>
#include <cmath>

#include <nine_morris_3d_engine/external/imgui.h++>
#include <nine_morris_3d_engine/external/glm.h++>

#include "scenes/game_scene.hpp"
#include "game/board.hpp"
#include "window_size.hpp"

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

void GameAnalysis::update_evaluation_bar() {
    m_interpolation += 0.001f;
    m_interpolation = std::min(m_interpolation, 1.0f);
}

void GameAnalysis::evaluation_bar_window(const sm::Ctx& ctx, const GameScene& game_scene) {
    const float width {sm::utils::map(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(MIN_WIDTH),
        static_cast<float>(MAX_WIDTH),
        Ui::rem(2.0f),
        Ui::rem(2.2f)
    )};

    const float height {sm::utils::map(
        static_cast<float>(ctx.get_window_height()),
        static_cast<float>(MIN_HEIGHT),
        static_cast<float>(MAX_HEIGHT),
        Ui::rem(11.0f),
        Ui::rem(35.0f)
    )};

    const float left_offset {sm::utils::map(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(MIN_WIDTH),
        static_cast<float>(MAX_WIDTH),
        Ui::rem(1.0f),
        Ui::rem(2.0f)
    )};

    const float top_anchor {static_cast<float>(ctx.get_window_height()) / 2.0f - height / 2.0f};
    const float bottom_anchor {static_cast<float>(ctx.get_window_height()) / 2.0f + height / 2.0f};

    handle_game_over(game_scene);

    int score {};

    switch (m_score_type) {
        case ScoreType::Eval:
            score = m_score;
            break;
        case ScoreType::Win: {
            // If that score means nothing, use the other one
            const int score_winner {m_score != 0 ? m_score : m_score_win};

            score = game_scene.score_bound() * (score_winner > 0 ? 1 : -1);
            break;
        }
    }

    if (m_score != m_score_old) {
        m_interpolation = 0.0f;
        m_score_old = m_score;
    }

    // Based on the white fill (absolute height), we can determine the black fill
    m_white_fill_real = sm::utils::map(
        static_cast<float>(score),
        -static_cast<float>(game_scene.score_bound()),
        static_cast<float>(game_scene.score_bound()),
        0.0f,
        height
    );

    m_white_fill_current = std::lerp(m_white_fill_current, m_white_fill_real, m_interpolation);

    ImDrawList* list {ImGui::GetBackgroundDrawList()};

    // Outline
    list->AddRectFilled(ImVec2(left_offset - 1.0f, top_anchor - 1.0f), ImVec2(left_offset + width + 1.0f, bottom_anchor + 1.0f), IM_COL32_BLACK, 6.0f);

    // White bar
    if (bottom_anchor - m_white_fill_current < bottom_anchor - 2.0f) {
        list->AddRectFilled(ImVec2(left_offset, bottom_anchor - m_white_fill_current), ImVec2(left_offset + width, bottom_anchor), game_scene.white_color(), 6.0f);
    }

    // Black bar
    if (top_anchor + height - m_white_fill_current > top_anchor + 2.0f) {
        list->AddRectFilled(ImVec2(left_offset, top_anchor), ImVec2(left_offset + width, top_anchor + height - m_white_fill_current), game_scene.black_color(), 6.0f);
    }

    // Middle
    list->AddLine(ImVec2(left_offset, top_anchor + height / 2.0f), ImVec2(left_offset + width, top_anchor + height / 2.0f), IM_COL32_BLACK);

    // If there is forced win, also render the moves needed to win in the opposite sides with opposite colors
    if (m_score_type == ScoreType::Win) {
        const auto text_win {std::to_string(std::abs(m_score))};
        const auto text_size {ImGui::CalcTextSize(text_win.c_str())};

        // If that score means nothing, use the other one
        const int score_winner {m_score != 0 ? m_score : m_score_win};

        if (score_winner > 0) {
            list->AddText(ImVec2(left_offset + width / 2.0f - text_size.x / 2.0f, top_anchor), game_scene.black_color(), text_win.c_str());
        } else {
            list->AddText(ImVec2(left_offset + width / 2.0f - text_size.x / 2.0f, bottom_anchor - text_size.y), game_scene.white_color(), text_win.c_str());
        }
    }
}

void GameAnalysis::handle_game_over(const GameScene& game_scene) {
    switch (game_scene.board().get_game_over()) {
        case GameOver::None:
            break;
        case GameOver::WinnerWhite:
            m_score_win = 1;
            break;
        case GameOver::WinnerBlack:
            m_score_win = -1;
            break;
        case GameOver::Draw:
            break;
    }

    if (game_scene.board().get_game_over() != GameOver::None) {
        m_pv.clear();
    }
}

void GameAnalysis::information_callback(const UciLikeEngine::Info& info) {
    if (info.score) {
        switch (info.score->index()) {
            case 0:
                m_score = std::get<0>(*info.score).value;
                m_score_type = ScoreType::Eval;
                break;
            case 1:
                m_score = std::get<1>(*info.score).value;
                m_score_type = ScoreType::Win;
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
