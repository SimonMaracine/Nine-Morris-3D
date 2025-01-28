#include "move_list.hpp"

#include <nine_morris_3d_engine/external/imgui.h++>

void MoveList::push(const std::string& string) {
    m_moves.push_back(string);
}

void MoveList::clear() {
    m_moves.clear();
    m_skip_first = false;
}

void MoveList::skip_first(bool skip) {
    m_skip_first = skip;
}

void MoveList::moves_window() const {
    if (ImGui::BeginChild("Move List")) {
        if (ImGui::BeginTable("Move Table", 3)) {
            if (m_skip_first) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d.", 1);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("--/--");

                for (std::size_t i {0}; i < m_moves.size(); i++) {
                    if (i % 2 == 0) {
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", m_moves[i].c_str());
                    } else {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%lu.", i / 2 + 2);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", m_moves[i].c_str());
                    }
                }
            } else {
                for (std::size_t i {0}; i < m_moves.size(); i++) {
                    if (i % 2 == 0) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%lu.", i / 2 + 1);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", m_moves[i].c_str());
                    } else {
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", m_moves[i].c_str());
                    }
                }
            }

            ImGui::EndTable();
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 6.0f) {
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();
}
