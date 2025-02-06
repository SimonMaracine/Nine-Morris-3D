#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstddef>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/utility.hpp>

namespace protocol {
    /*
        When a client has game over, it immediately destroys its session and sends Client_LeaveGameSession.

        Leaving a session is either voluntarily or involuntarily.

        When a client voluntarily leaves the session, it sends Client_LeaveGameSession.
        Voluntarily leaving a session means resigning, except if the other client has already left the session,
        or it hasn't even joined the session.

        Involuntarily leaving the session lets the client rejoin and continue the game.

        When a client leaves the session, the other client is notified with Server_RemoteLeaveGameSession.
    */

    /*
        Client_Ping
            Test if the server is still alive. The server sends back a reply with the same data, with
            Server_Ping.

        Server_Ping
            Reply a Client_Ping message.

        Client_RequestGameSession
            Request a new game session. Block in a modal window, waiting for a reply from the server.
            A game session means just one round of game. The server accepts or rejects the request, with
            Server_AcceptGameSession and Server_RejectGameSession. Client_RequestGameSession is called
            when the client presses the start game button.

            A session is destroyed by the server when the last client leaves the session.

        Server_AcceptGameSession
            Create a new session. The client then creates the session as well and blocks in a modal window,
            waiting for the remote to join. It may stop waiting and leave the session by pressing the
            cancel game button, sending Client_LeaveGameSession.

        Server_RejectGameSession
            Fail to create a new session. Send an error code.

        Client_RequestJoinGameSession
            Request to join a game session. Block in a modal window, waiting for a reply from the server.
            The server accepts or rejects the request, with Server_AcceptJoinGameSession and Server_RejectJoinGameSession.
            It is called when the client presses the join game button.

        Server_AcceptJoinGameSession
            Acknowledge a game session with that specific ID. The client unblocks and the game is ready to start.
            The client receives the played moves so far, enabling it to continue an interrupted game. It also
            receives the messages. An involuntarily disconnected client may rejoin the session.

        Server_RejectJoinGameSession
            Fail to find a session with that specific ID. Send an error code.

        Server_RemoteJoinedGameSession
            Notify the client that the remote has joined the session. The client unblocks and the game is ready
            to start.

        Client_LeaveGameSession
            Voluntarily leave the session. It is called when the client presses the new game button, or the
            cancel game button while waiting for the remote, or when changing the game, or quitting the application.
            It is not sent when the application crashes or it disconnects from the server.
            The remote is notified about the forfeit in any case, with Server_RemoteLeaveGameSession.

        Server_RemoteLeaveGameSession
            Notify the client that the remote has either voluntarily, or involuntarily left the session.
            If the remote rejoins, the client is notified.

        Client_PlayMove
            Play a move on the board. The server remembers the played move and notifies the remote with
            Server_RemotePlayedMove. The player's whose turn just ended synchronizes remote's clock with its own
            clock. The time on the clock is also remembered by the server.

        Server_RemotePlayedMove
            Notify the client that the remote has played a move.

        Client_UpdateTurnTime
            Update the running clock of the turn on the server. It is called only by the client that has the turn,
            i.e. that has the clock running. The other client's clock, being stopped, is already correct on the
            server, but the running clock isn't. If the client having the turn disconnected, the server could
            have lost the time, and when it rejoined, it would have extra time. This protocol assures that the
            clocks on the server are almost correct.

        Client_Resign
            End the game by losing. It is called when the client presses the resign button, or when it voluntarily
            leaves the session while the remote is still present in the session. The server notifies the remote
            with Server_RemoteResigned.

        Server_RemoteResigned
            Notify the client that the remote has resigned.

        Client_OfferDraw
            Offer a draw. It is called only when the client presses the offer draw button. The server notifies
            the remote with Server_RemoteOfferedDraw.

        Server_RemoteOfferedDraw
            Notify the client that the remote has offered a draw. The client then has the opportunity to
            accept the draw by pressing the accept draw button. It loses the opportunity by playing a move.

        Client_AcceptDraw
            Accept the draw by pressing the accept draw button. The game is over.
            The server notifies the remote with Server_RemoteAcceptedDraw.

        Server_RemoteAcceptedDraw
            Notify the client that the remote has accepted the draw. The game is over.

        Client_SendMessage
            Sends a message. The server notifies the remote with Server_RemoteSentMessage.

        Server_RemoteSentMessage
            Notify the client that the remote has sent a message.
    */

    namespace message {
        enum MessageType : std::uint16_t {
            Client_Ping,
            Server_Ping,

            Client_RequestGameSession,
            Server_AcceptGameSession,
            Server_RejectGameSession,

            Client_RequestJoinGameSession,
            Server_AcceptJoinGameSession,
            Server_RejectJoinGameSession,

            Server_RemoteJoinedGameSession,

            Client_LeaveGameSession,
            Server_RemoteLeaveGameSession,

            Client_PlayMove,
            Server_RemotePlayedMove,

            Client_UpdateTurnTime,

            Client_Resign,
            Server_RemoteResigned,

            Client_OfferDraw,
            Server_RemoteOfferedDraw,
            Client_AcceptDraw,
            Server_RemoteAcceptedDraw,

            Client_SendMessage,
            Server_RemoteSentMessage
        };
    }

    using SessionId = std::uint16_t;
    using TimePoint = std::chrono::system_clock::time_point;
    using ClockTime = unsigned int;

    using Messages = std::vector<std::pair<std::string, std::string>>;
    inline constexpr std::size_t MAX_MESSAGE_SIZE {128};

    enum class Player {
        White,
        Black
    };

    enum class GameMode {
        NineMensMorris,
        TwelveMensMorris
    };

    enum class ErrorCode {
        TooManySessions,
        InvalidSessionId,
        SessionOccupied,
        SessionExpired,
        SessionDifferentGame
    };

    inline const char* error_code_string(ErrorCode error_code) {
        const char* string {};

        switch (error_code) {
            case ErrorCode::TooManySessions:
                string = "There is no room for another session";
                break;
            case ErrorCode::InvalidSessionId:
                string = "No session could be found";
                break;
            case ErrorCode::SessionOccupied:
                string = "The session is occupied";
                break;
            case ErrorCode::SessionExpired:
                string = "The session has expired";
                break;
            case ErrorCode::SessionDifferentGame:
                string = "The session has a different game than the one requested";
                break;
        }

        return string;
    }

    inline Player opponent(Player player) {
        if (player == Player::White) {
            return Player::Black;
        } else {
            return Player::White;
        }
    }

    struct Client_Ping {
        TimePoint time;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(time);
        }
    };

    struct Server_Ping {
        TimePoint time;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(time);
        }
    };

    struct Client_RequestGameSession {
        std::string player_name;
        Player remote_player {};
        ClockTime time {};  // Both players' times
        protocol::GameMode game_mode {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(player_name, remote_player, time, game_mode);
        }
    };

    struct Server_AcceptGameSession {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_RejectGameSession {
        ErrorCode error_code {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(error_code);
        }
    };

    struct Client_RequestJoinGameSession {
        SessionId session_id {};
        std::string player_name;
        protocol::GameMode game_mode {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, player_name, game_mode);
        }
    };

    struct Server_AcceptJoinGameSession {
        SessionId session_id {};  // Not really needed, but simplifies implementation
        Player remote_player {};
        ClockTime remote_time {};
        ClockTime time {};
        std::vector<std::string> moves;
        Messages messages;
        std::string remote_name;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, remote_player, remote_time, time, moves, messages, remote_name);
        }
    };

    struct Server_RejectJoinGameSession {
        ErrorCode error_code {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(error_code);
        }
    };

    struct Server_RemoteJoinedGameSession {
        std::string remote_name;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(remote_name);
        }
    };

    struct Client_LeaveGameSession {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_RemoteLeaveGameSession {};

    struct Client_PlayMove {
        SessionId session_id {};
        ClockTime time {};  // After player's turn
        std::string move;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, time, move);
        }
    };

    struct Server_RemotePlayedMove {
        ClockTime time {};  // After player's turn
        std::string move;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(time, move);
        }
    };

    struct Client_UpdateTurnTime {
        SessionId session_id {};
        ClockTime time {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, time);
        }
    };

    struct Client_Resign {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_RemoteResigned {};

    struct Client_OfferDraw {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_RemoteOfferedDraw {};

    struct Client_AcceptDraw {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_RemoteAcceptedDraw {};

    struct Client_AcknowledgeGameOver {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_AcknowledgeGameOver {};

    struct Client_Rematch {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Client_SendMessage {
        SessionId session_id {};
        std::string message;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, message);
        }
    };

    struct Server_RemoteSentMessage {
        std::string message;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(message);
        }
    };
}
