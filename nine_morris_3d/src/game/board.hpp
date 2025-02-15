#pragma once

#include <functional>
#include <vector>
#include <utility>
#include <string>
#include <stdexcept>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "hoverable.hpp"
#include "player_color.hpp"

// Type that keeps track of the state and result of a game
class GameOver {
public:
    enum Type : int {
        None,
        WinnerWhite,
        WinnerBlack,
        Draw
    };

    GameOver() = default;
    GameOver(Type type, const std::string& reason)
        : m_type(type), m_reason(reason) {}

    operator int() const { return m_type; }
    operator const std::string&() const { return m_reason; }

    const char* to_string() const;
private:
    Type m_type {None};
    std::string m_reason;
};

// Generic board object representing a game with its data and rules
class BoardObj {
public:
    BoardObj() = default;
    virtual ~BoardObj() = default;

    BoardObj(const BoardObj&) = default;
    BoardObj& operator=(const BoardObj&) = default;
    BoardObj(BoardObj&&) = default;
    BoardObj& operator=(BoardObj&&) = default;

    virtual void user_click_release_callback() = 0;
    virtual const GameOver& get_game_over() const = 0;
    virtual PlayerColor get_player_color() const = 0;
    virtual bool is_turn_finished() const = 0;
    virtual void setup_pieces(bool animate = true) = 0;

    void user_click_press();
    void user_click_release();
    void enable_move_callback(bool enable);
    void enable_move_animations(bool enable);
protected:
    void update_hover_id(glm::vec3 ray, glm::vec3 camera, std::function<std::vector<HoverableObj>()>&& get_hoverables);

    static std::string format(const char* format, ...);

    bool m_enable_move_callback {true};
    bool m_enable_move_animations {true};
    int m_click_id {-1};
    int m_hover_id {-1};
    GameOver m_game_over;
};

// Generic error thrown inside board code
struct BoardError : std::runtime_error {
    explicit BoardError(const char* message)
        : std::runtime_error(message) {}
    explicit BoardError(const std::string& message)
        : std::runtime_error(message) {}
};
