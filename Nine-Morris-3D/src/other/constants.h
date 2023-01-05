#pragma once

#include <engine/engine_graphics.h>

// format is the first argument to __VA_ARGS__
#define FORMATTED_MESSAGE(result, size, ...) \
    char result[size]; \
    snprintf(result, size, __VA_ARGS__);

#define TURN_IS_WHITE_SO(_true, _false) (turn == BoardPlayer::White ? (_true) : (_false))
#define WAIT_FOR_NEXT_MOVE() next_move = false
#define CAN_MAKE_MOVE() next_move = true

#define PIECE_Y_FLOATING_POSITION 0.3f
#define WHITE_PIECE_POSITION(i) glm::vec3(-4.0f, PIECE_Y_FLOATING_POSITION, -2.0f + (i) * 0.49f)
#define BLACK_PIECE_POSITION(i) glm::vec3(4.0f, PIECE_Y_FLOATING_POSITION, -2.0f + ((i) - 9) * 0.49f)
#define RANDOM_PIECE_ROTATION() glm::vec3(0.0f, glm::radians(static_cast<float>(rand() % 360)), 0.0f)
#define PIECE_INDEX_POSITION(i) glm::vec3(NODE_POSITIONS[i].x, PIECE_Y_POSITION, NODE_POSITIONS[i].z)

#define DEFAULT_BROWN ImVec4(0.647f, 0.4f, 0.212f, 1.0f)
#define DARK_BROWN ImVec4(0.4f, 0.25f, 0.1f, 1.0f)
#define LIGHT_BROWN ImVec4(0.68f, 0.48f, 0.22f, 1.0f)
#define BEIGE ImVec4(0.961f, 0.875f, 0.733f, 1.0f)
#define LIGHT_GRAY_BLUE ImVec4(0.357f, 0.408f, 0.525f, 1.0f)
#define TRANSPARENT_BACKGROUND ImVec4(0.058f, 0.058f, 0.058f, 0.74f)

constexpr size_t NINE_MENS_MORRIS_MILLS = 16;
constexpr size_t MILLS_NINE_MENS_MORRIS[NINE_MENS_MORRIS_MILLS][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

constexpr size_t TWELVE_MENS_MORRIS_MILLS = 20;
constexpr size_t MILLS_TWELVE_MENS_MORRIS[TWELVE_MENS_MORRIS_MILLS][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 },
    { 0, 3, 6 }, { 2, 5, 8 }, { 15, 18, 21 }, { 17, 20, 23 }
};

constexpr DirectionalLight LIGHT_FIELD = {
    glm::vec3(5.7f, 8.4f, 12.4f),
    glm::vec3(0.4f),
    glm::vec3(0.88f),
    glm::vec3(0.9f)
};

constexpr Renderer::LightSpace SHADOWS_FIELD = {
    -4.7f, 4.7f,
    -1.9f, 2.76f,
    1.0f, 9.0f,
    3.1f
};

constexpr DirectionalLight LIGHT_AUTUMN = {
    glm::vec3(-4.4f, 11.0f, 6.4f),
    glm::vec3(0.32f),
    glm::vec3(0.82f),
    glm::vec3(0.82f)
};

constexpr Renderer::LightSpace SHADOWS_AUTUMN = {
    -4.66f, 4.66f,
    -3.24f, 4.29f,
    1.0f, 9.0f,
    3.1f
};

constexpr DirectionalLight LIGHT_NONE = {  // TODO right now they are the same as field; change this
    glm::vec3(5.7f, 8.4f, 12.4f),
    glm::vec3(0.4f),
    glm::vec3(0.88f),
    glm::vec3(0.9f)
};

constexpr Renderer::LightSpace SHADOWS_NONE = {
    -4.7f, 4.7f,
    -1.9f, 2.76f,
    1.0f, 9.0f,
    3.1f
};

enum class BoardPhase {
    None,
    PlacePieces = 1,
    MovePieces = 2,
    GameOver
};

enum class BoardPlayer {
    White = 0,
    Black = 1
};

struct BoardEnding {
    enum Type {
        None,
        WinnerWhite,
        WinnerBlack,
        TieBetweenBothPlayers
    };

    Type type = None;
    std::string reason;
};

enum class PieceType {
    White = 0,
    Black = 1,
    None
};

enum class PieceMovementType {
    None,
    Linear,
    ThreeStep
};

enum class GamePlayer {
    None,
    Human,
    Computer
};

enum class GameState {  // TODO this can be better + more states needed for Computer
    MaybeNextPlayer,
    HumanBeginMove,
    HumanThinkingMove,
    HumanDoingMove,
    HumanEndMove,
    ComputerBeginMove,
    ComputerThinkingMove,
    ComputerDoingMove,
    ComputerEndMove
};

enum class KeyboardControlsDirection {
    Up = 0,
    Left = 1,
    Down = 2,
    Right = 3
};

enum class Skybox {
    None,
    Field,
    Autumn
};

enum class TextureQuality {
    Normal,
    Low
};

enum class GameMode {
    Standard,
    Jump,
    JumpPlus
};

enum class WindowImGui {
    None,
    ShowAbout,
    ShowCouldNotLoadGame,
    ShowNoLastGame,
    ShowGameOver,
    ShowRulesStandardGame,
    ShowRulesJumpVariant,
    ShowRulesJumpPlusVariant
};

using Index = int;
constexpr Index NULL_INDEX = -1;

constexpr size_t MAX_NODES = 24;
constexpr size_t MAX_PIECES = 18;  // TODO will change

constexpr unsigned int MAX_TURNS_WITHOUT_MILLS = 40;
constexpr float PAINT_Y_POSITION = 0.062f;

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float PIECE_BASE_VELOCITY = 0.3f;
constexpr float PIECE_VARIABLE_VELOCITY = 10.0f;
constexpr float PIECE_THREESTEP_HEIGHT = 0.4f;
constexpr float PIECE_RAISE_HEIGHT = 1.3f;

constexpr float NODE_Y_POSITION = 0.063f;
constexpr glm::vec3 NODE_POSITIONS[MAX_NODES] = {
    glm::vec3(2.046f, NODE_Y_POSITION, 2.062f),    // 0
    glm::vec3(-0.008f, NODE_Y_POSITION, 2.089f),   // 1
    glm::vec3(-2.101f, NODE_Y_POSITION, 2.076f),   // 2
    glm::vec3(1.480f, NODE_Y_POSITION, 1.512f),    // 3
    glm::vec3(0.001f, NODE_Y_POSITION, 1.513f),    // 4
    glm::vec3(-1.509f, NODE_Y_POSITION, 1.502f),   // 5
    glm::vec3(0.889f, NODE_Y_POSITION, 0.898f),    // 6
    glm::vec3(0.001f, NODE_Y_POSITION, 0.906f),    // 7
    glm::vec3(-0.930f, NODE_Y_POSITION, 0.892f),   // 8
    glm::vec3(2.058f, NODE_Y_POSITION, 0.031f),    // 9
    glm::vec3(1.481f, NODE_Y_POSITION, 0.025f),    // 10
    glm::vec3(0.894f, NODE_Y_POSITION, 0.026f),    // 11
    glm::vec3(-0.934f, NODE_Y_POSITION, 0.050f),   // 12
    glm::vec3(-1.508f, NODE_Y_POSITION, 0.050f),   // 13
    glm::vec3(-2.083f, NODE_Y_POSITION, 0.047f),   // 14
    glm::vec3(0.882f, NODE_Y_POSITION, -0.894f),   // 15
    glm::vec3(0.011f, NODE_Y_POSITION, -0.900f),   // 16
    glm::vec3(-0.930f, NODE_Y_POSITION, -0.890f),  // 17
    glm::vec3(1.477f, NODE_Y_POSITION, -1.455f),   // 18
    glm::vec3(0.006f, NODE_Y_POSITION, -1.463f),   // 19
    glm::vec3(-1.493f, NODE_Y_POSITION, -1.458f),  // 20
    glm::vec3(2.063f, NODE_Y_POSITION, -2.046f),   // 21
    glm::vec3(0.001f, NODE_Y_POSITION, -2.061f),   // 22
    glm::vec3(-2.081f, NODE_Y_POSITION, -2.045f)   // 23
};

constexpr glm::vec3 PIECE_BOUNDING_BOX = glm::vec3(0.36f, 0.19f, 0.36f);
constexpr glm::vec3 NODE_BOUNDING_BOX = glm::vec3(0.32f, 0.01f, 0.32f);
constexpr glm::vec3 BOARD_BOUNDING_BOX = glm::vec3(5.5f, 0.01f, 5.5f);

constexpr float KEYBOARD_CONTROLS_Y_POSITION = 0.47f;

using GamePosition = std::array<PieceType, MAX_NODES>;

constexpr float WORLD_SCALE = 20.0f;

constexpr glm::vec3 RED_OUTLINE = { 1.0f, 0.0f, 0.0f };
constexpr glm::vec3 ORANGE_OUTLINE = { 1.0f, 0.5f, 0.0f };
constexpr glm::vec3 RED_TINT = { 1.0f, 0.2f, 0.2f };
constexpr glm::vec3 DEFAULT_TINT = { 1.0f, 1.0f, 1.0f };
constexpr glm::vec4 GRAY = { 0.7f, 0.7f, 0.7f, 1.0f };

constexpr glm::vec3 UP_VECTOR = { 0.0f, 1.0f, 0.0f };

constexpr int WIDGET_LOWEST_RESOLUTION = 288;
constexpr int WIDGET_HIGHEST_RESOLUTION = 1035;
