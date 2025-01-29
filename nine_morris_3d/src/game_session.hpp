#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <protocol.hpp>

#include "global.hpp"

class GameScene;

class GameSession {
public:
    explicit GameSession(protocol::SessionId session_id)
        : m_session_id(session_id) {}

    protocol::SessionId get_session_id() const { return m_session_id; }
    void set_messages(const protocol::Messages& messages) { m_messages = messages; }

    void remote_join(const std::string& player_name);
    void remote_quit();

    void remote_sent_message(const std::string& message);

    void session_window(GameScene& game_scene, const Global& g);
private:
    bool send_message_available() const;

    protocol::SessionId m_session_id {};
    bool m_remote_joined {false};
    std::string m_remote_player_name;
    protocol::Messages m_messages;
    char m_message_buffer[protocol::MAX_MESSAGE_SIZE] {};
};
