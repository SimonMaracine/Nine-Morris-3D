#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <protocol.hpp>

class GameScene;

class GameSession {
public:
    explicit GameSession(protocol::SessionId session_id)
        : m_session_id(session_id) {}

    protocol::SessionId get_session_id() const { return m_session_id; }
    bool get_remote_joined() const { return m_remote_joined; }
    bool get_remote_offered_draw() const { return m_remote_offered_draw; }
    bool set_remote_offered_draw(bool remote_offered_draw) { return m_remote_offered_draw = remote_offered_draw; }
    void set_messages(const protocol::Messages& messages) { m_messages = messages; }

    void remote_joined(const std::string& player_name);
    void remote_left();
    void remote_sent_message(const std::string& message);

    void session_window(sm::Ctx& ctx, GameScene& game_scene);
private:
    bool send_message_available() const;

    protocol::SessionId m_session_id {};
    bool m_remote_joined {false};
    bool m_remote_offered_draw {false};
    std::string m_remote_player_name;
    protocol::Messages m_messages;
    char m_message_buffer[protocol::MAX_MESSAGE_SIZE] {};
};
