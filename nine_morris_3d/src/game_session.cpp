#include "game_session.hpp"

#include <cstring>

#include <nine_morris_3d_engine/external/imgui.h++>

#include "scenes/game_scene.hpp"
#include "ui.hpp"
#include "global.hpp"
#include "window_size.hpp"

using namespace sm::localization_literals;

void GameSession::remote_joined(const std::string& player_name) {
    m_remote_joined = true;
    m_remote_player_name = player_name;
}

void GameSession::remote_left() {
    m_remote_joined = false;
    m_remote_player_name.clear();
}

void GameSession::remote_sent_message(const std::string& message) {
    m_messages.emplace_back(m_remote_player_name, message);
}

void GameSession::session_window(sm::Ctx& ctx, GameScene& game_scene) {
    const float width {sm::utils::map(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(MIN_WIDTH),
        static_cast<float>(MAX_WIDTH),
        Ui::rem(11.0f),
        Ui::rem(15.0f)
    )};

    const float height {sm::utils::map(
        static_cast<float>(ctx.get_window_height()),
        static_cast<float>(MIN_HEIGHT),
        static_cast<float>(MAX_HEIGHT),
        Ui::rem(10.0f),
        Ui::rem(14.0f)
    )};

    const float left_offset {sm::utils::map(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(MIN_WIDTH),
        static_cast<float>(MAX_WIDTH),
        Ui::rem(1.0f),
        Ui::rem(2.0f)
    )};

    const float down_offset {sm::utils::map(
        static_cast<float>(ctx.get_window_height()),
        static_cast<float>(MIN_HEIGHT),
        static_cast<float>(MAX_HEIGHT),
        Ui::rem(1.0f),
        Ui::rem(2.0f)
    )};

    ImGui::SetNextWindowPos(
        ImVec2(left_offset, static_cast<float>(ctx.get_window_height()) - down_offset),
        ImGuiCond_Always,
        ImVec2(0.0f, 1.0f)
    );

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    if (ImGui::Begin("##Session", nullptr, ImGuiWindowFlags_NoDecoration)) {
        if (m_remote_joined) {
            ImGui::TextWrapped("%s %s.", "Playing against"_L, m_remote_player_name.empty() ? "an unnamed opponent"_L : m_remote_player_name.c_str());

            if (m_remote_offered_draw) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 163, 71, 255));
                ImGui::TextWrapped("%s %s.", m_remote_player_name.empty() ? "The opponent"_L : m_remote_player_name.c_str(), "has offered a draw"_L);
                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
                    ImGui::SetTooltip("%s", "remote_offered_draw_tooltip"_L);
                }
            }

            if (game_scene.get_game_state() == GameState::Over) {
                ImGui::Dummy(ImVec2(0.0f, Ui::rem(0.1f)));

                if (ImGui::Button("Rematch"_L)) {
                    game_scene.client_rematch();
                }
            }
        } else if (game_scene.get_game_state() != GameState::Ready) {
            ImGui::Image(game_scene.get_icon_wait()->get_id(), ImVec2(Ui::rem(1.0f), Ui::rem(1.0f)));
            ImGui::SameLine();
            ImGui::TextWrapped("%s", "The opponent has disconnected."_L);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
                ImGui::SetTooltip("%s", "opponent_disconnected_tooltip"_L);
            }
        } else {
            ImGui::Image(game_scene.get_icon_wait()->get_id(), ImVec2(Ui::rem(1.0f), Ui::rem(1.0f)));
            ImGui::SameLine();
            ImGui::TextWrapped("%s", "Waiting for the opponent..."_L);
        }

        ImGui::Dummy(ImVec2(0.0f, Ui::rem(0.1f)));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, Ui::rem(0.1f)));

        const float chat_height {Ui::rem(2.5f)};

        if (ImGui::BeginChild("##Messages", ImVec2(0.0f, ImGui::GetContentRegionAvail().y - chat_height))) {
            std::string last_name {!m_messages.empty() ? m_messages.front().first : ""};

            for (const auto& message : m_messages) {
                if (message.first != last_name) {
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::TextWrapped("%s", message.second.c_str());

                    last_name = message.first;
                } else {
                    ImGui::TextWrapped("%s", message.second.c_str());
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                    ImGui::SetTooltip("%s", message.first.c_str());
                }
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - Ui::rem(0.3f)) {
                ImGui::SetScrollHereY(1.0f);
            }
        }

        ImGui::EndChild();

        const auto& g {ctx.global<Global>()};
        const auto& style {ImGui::GetStyle()};

        const float button_width {ImGui::CalcTextSize("Send"_L).x + style.FramePadding.x * 2};
        const auto size {ImVec2(ImGui::GetContentRegionAvail().x - style.ItemSpacing.x - button_width, ImGui::GetContentRegionAvail().y)};
        const auto flags {ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_EnterReturnsTrue};

        if (ImGui::InputTextMultiline("##MessageBox", m_message_buffer, sizeof(m_message_buffer), size, flags)) {
            if (send_message_available()) {
                game_scene.client_send_message(m_message_buffer);
                m_messages.emplace_back(g.options.name, m_message_buffer);
                std::memset(m_message_buffer, 0, sizeof(m_message_buffer));
                ImGui::SetKeyboardFocusHere(-1);
            }
        }

        ImGui::SameLine();

        ImGui::BeginDisabled(!send_message_available());
        if (ImGui::Button("Send"_L, ImGui::GetContentRegionAvail())) {
            game_scene.client_send_message(m_message_buffer);
            m_messages.emplace_back(g.options.name, m_message_buffer);
            std::memset(m_message_buffer, 0, sizeof(m_message_buffer));
        }
        ImGui::EndDisabled();
    }

    ImGui::End();
}

bool GameSession::send_message_available() const {
    return m_message_buffer[0] != 0;
}
