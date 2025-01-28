#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <protocol.hpp>

class GameScene;

class GameSession {
public:
    explicit GameSession(protocol::SessionId session_id)
        : m_session_id(session_id) {}

    protocol::SessionId get_session_id() const { return m_session_id; }

    void remote_join(const std::string& player_name);
    void remote_quit();

    void session_window(GameScene& game_scene);
private:
    protocol::SessionId m_session_id {};
    bool m_remote_joined {false};
    std::string m_remote_player_name;
    protocol::Messages m_messages;
    char m_message_buffer[protocol::MAX_MESSAGE_SIZE] {};
};
