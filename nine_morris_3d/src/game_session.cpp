#include "game_session.hpp"

#include <cstring>

#include <nine_morris_3d_engine/external/imgui.h++>

#include "scenes/game_scene.hpp"
#include "ui.hpp"
#include "global.hpp"
#include "window_size.hpp"

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
        Ui::rem(9.0f),
        Ui::rem(13.0f)
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

    if (ImGui::Begin("Session", nullptr, ImGuiWindowFlags_NoDecoration)) {
        if (m_remote_joined) {
            ImGui::TextWrapped("Playing against %s.", m_remote_player_name.empty() ? "an unnamed opponnent" : m_remote_player_name.c_str());

            if (m_remote_offered_draw) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 163, 71, 255));
                ImGui::TextWrapped("%s has offered a draw.", m_remote_player_name.empty() ? "The opponnent" : m_remote_player_name.c_str());
                ImGui::PopStyleColor();
            }

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
                ImGui::SetTooltip("You may accept the draw from the menu.");
            }
        } else if (game_scene.get_game_state() != GameState::Ready) {
            ImGui::TextWrapped("The opponent has disconnected. You may wait for them to rejoin.");
        } else {
            ImGui::TextWrapped("Waiting for the opponent...");
        }

        ImGui::Dummy(ImVec2(0.0f, Ui::rem(0.1f)));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, Ui::rem(0.1f)));

        const float chat_height {Ui::rem(2.5f)};

        if (ImGui::BeginChild("Messages", ImVec2(0.0f, ImGui::GetContentRegionAvail().y - chat_height))) {
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

        const float button_width {Ui::rem(3.0f)};
        const auto size {ImVec2(ImGui::GetContentRegionAvail().x - button_width, ImGui::GetContentRegionAvail().y)};
        const auto flags {ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_EnterReturnsTrue};

        if (ImGui::InputTextMultiline("##", m_message_buffer, sizeof(m_message_buffer), size, flags)) {
            if (send_message_available()) {
                game_scene.client_send_message(m_message_buffer);
                m_messages.emplace_back(g.options.name, m_message_buffer);
                std::memset(m_message_buffer, 0, sizeof(m_message_buffer));
                ImGui::SetKeyboardFocusHere(-1);
            }
        }

        ImGui::SameLine();

        ImGui::BeginDisabled(!send_message_available());
        if (ImGui::Button("Send", ImGui::GetContentRegionAvail())) {
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
