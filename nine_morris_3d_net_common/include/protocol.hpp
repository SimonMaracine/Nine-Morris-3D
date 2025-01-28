#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/chrono.hpp>

namespace protocol {
    /*
        Client_Ping
            The client tests if the server is still alive. The server sends back a reply with the same data.

        Server_Ping
            The server replies to a client ping.

        Client_RequestGameSession
            The client presses the start new game button. The server accepts or denies the request. The client then
            blocks in a modal window, waiting for the server to respond. It may stop waiting and quit the session by
            pressing the cancel game button.

        Server_AcceptGameSession
            The server creates a new session. The client then blocks in a modal window, waiting
            for the remote to join. It may stop waiting and quit the session by pressing the cancel game button.

        Server_DenyGameSession
            The server fails to create a new session.

        Client_RequestJoinGameSession
            The client presses the join game button with a specific ID. It then blocks in modal window, waiting for
            the server to respond. The server may deny the request, or accept it.

        Server_AcceptJoinGameSession
            The server acknowledges a game session with that specific ID. The client unblocks and the game is ready.
            The client receives the played moves so far, enabling it to continue an interrupted game.

        Server_DenyJoinGameSession
            The server fails to find a session with that specific ID.

        Server_RemoteJoinedGameSession
            The server informs the client that the remote has joined. The client unblocks and the game is ready.

        Client_QuitGameSession
            The client either presses the cancel game button, the quit session button, starts a new session,
            or starts a new local game.

        Server_RemoteQuitGameSession
            The server informs the client that the remote has either voluntarily quit the session, or disconnected.
            The client blocks in a modal window, either waiting for the remote to rejoin the session, or to quit
            the session as well. If the remote rejoins, the client is informed.

            When the last client quits a session, it gets destroyed by the server.

        Client_PlayMove
            The client plays a move on the board. The server remembers the played move.

        Server_RemotePlayedMove
            The server informs the client that the remote has played a move.

        Client_AcknowledgeGameOver
            The client acknowledges that the game is over. When both players acknowledge this fact, the server
            acknowledges it too.

        Server_AcknowledgeGameOver
            The server acknowledges that both clients know that the game is over. It sends this message to both.

        Client_Rematch
            After a game is over (server acknowledged game over), the client presses the rematch button and blocks
            in a modal window, waiting for the remote to do the same.

        Server_Rematch
            After a game is over (server acknowledged game over), the server acknowledges that both clients want
            a rematch. It sends this message to both. Both clients unblock and the game is ready.
    */

    namespace message {
        enum MessageType : std::uint16_t {
            Client_Ping,  // Client wanted to know if the server is alive
            Server_Ping,  // Server has responded back

            Client_RequestGameSession,  // Client has pressed the start new game button
            Server_AcceptGameSession,  // Server has accepted and started a new session
            Server_DenyGameSession,  // Server has denied the new session

            Client_RequestJoinGameSession,  // Client has pressed the join game button
            Server_AcceptJoinGameSession,  // Server has accepted the join (game is ready)
            Server_DenyJoinGameSession,  // Server has denied the join

            Server_RemoteJoinedGameSession,  // Remote client has pressed the join game button (game is ready)

            Client_QuitGameSession,  // Client has quit the session
            Server_RemoteQuitGameSession,  // Remote client has quit the session

            Client_PlayMove,  // Client has played a move
            Server_RemotePlayedMove,  // Remote client has played a move

            // Client_OfferDraw,  // Client has pressed the offer draw button
            // Server_RemoteOfferedDraw,  // Remote client has pressed the offer draw button
            // Client_AcceptDrawOffer,  // Client has pressed the accept draw offer button
            // Server_RemoteAcceptedDrawOffer,  // Remote client has pressed the accept draw offer button

            // Client_Resign,  // Client has pressed the resign button or the new game button
            // Server_RemoteResigned,  // Remote client has pressed the resign button or the new game button

            // Client_AcknowledgeGameOver,  // Client has acknowledged that the game is over on their side
            // Server_AcknowledgeGameOver,  // Server has acknowledged that both clients have acknowledged game over

            Client_Rematch,  // Client has pressed the rematch button (server has already acknowledged game over)
            Server_Rematch  // Both clients have pressed the rematch button (game is ready)
        };
    }

    using SessionId = std::uint16_t;

    using TimePoint = std::chrono::system_clock::time_point;

    enum class Player {
        White = 0,
        Black = 1
    };

    enum class ErrorCode {
        TooManySessions,
        InvalidSessionId,
        SessionOccupied
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
        Player remote_player_type {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(player_name, remote_player_type);
        }
    };

    struct Server_AcceptGameSession {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_DenyGameSession {
        ErrorCode error_code {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(error_code);
        }
    };

    struct Client_RequestJoinGameSession {
        SessionId session_id {};
        std::string player_name;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, player_name);
        }
    };

    struct Server_AcceptJoinGameSession {
        std::vector<std::string> moves;
        std::string remote_player_name;
        Player remote_player_type {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(moves, remote_player_name, remote_player_type);
        }
    };

    struct Server_DenyJoinGameSession {
        ErrorCode error_code {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(error_code);
        }
    };

    struct Server_RemoteJoinedGameSession {
        std::string remote_player_name;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(remote_player_name);
        }
    };

    struct Client_QuitGameSession {
        SessionId session_id {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id);
        }
    };

    struct Server_RemoteQuitGameSession {};

    struct Client_PlayMove {
        SessionId session_id {};
        std::string move;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(session_id, move);
        }
    };

    struct Server_RemotePlayedMove {
        std::string move;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(move);
        }
    };

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

    struct Server_Rematch {
        Player remote_player_type {};

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(remote_player_type);
        }
    };
}
