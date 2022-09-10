#pragma once

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

enum class BoardEnding {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
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

enum class GameState {
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

constexpr unsigned int MAX_TURNS_WITHOUT_MILLS = 40;
constexpr float PAINT_Y_POSITION = 0.062f;

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float PIECE_BASE_VELOCITY = 0.3f;
constexpr float PIECE_VARIABLE_VELOCITY = 10.0f;
constexpr float PIECE_THREESTEP_HEIGHT = 0.4f;
constexpr float PIECE_RAISE_HEIGHT = 1.3f;

constexpr float NODE_Y_POSITION = 0.063f;
constexpr glm::vec3 NODE_POSITIONS[24] = {
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

constexpr size_t INVALID_PIECE_INDEX = 18;
constexpr size_t INVALID_NODE_INDEX = 24;

constexpr float KEYBOARD_CONTROLS_Y_POSITION = 0.47f;

constexpr const char* NORMAL = "normal";
constexpr const char* LOW = "low";
constexpr const char* FIELD = "field";
constexpr const char* AUTUMN = "autumn";

constexpr int HUMAN = static_cast<int>(GamePlayer::Human);
constexpr int COMPUTER = static_cast<int>(GamePlayer::Computer);

using GamePosition = std::array<PieceType, 24>;
