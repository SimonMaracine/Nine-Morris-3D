#include "game_session.hpp"

#include <cstring>

#include <nine_morris_3d_engine/external/imgui.h++>

#include "scenes/game_scene.hpp"
#include "ui.hpp"

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

void GameSession::session_window(GameScene& game_scene, const Global& g) {
    if (ImGui::Begin("Session")) {
        if (m_remote_joined) {
            ImGui::TextWrapped("Playing against %s.", m_remote_player_name.empty() ? "an unnamed opponnent" : m_remote_player_name.c_str());
        } else {
            ImGui::TextWrapped("The opponent has left the session. You may wait for them to rejoin.");
        }

        ImGui::Separator();

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

        const float button_width {Ui::rem(3.0f)};
        const auto size {ImVec2(ImGui::GetContentRegionAvail().x - button_width, ImGui::GetContentRegionAvail().y)};
        const ImGuiInputTextFlags flags {ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_EnterReturnsTrue};

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
