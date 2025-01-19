#include "game/board.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cassert>

#include <nine_morris_3d_engine/external/glm.h++>

#include "game/ray.hpp"

static glm::mat4 transformation_matrix(glm::vec3 position, glm::vec3 rotation, float scale) {
    glm::mat4 matrix {1.0f};
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(scale));

    return matrix;
}

const char* GameOver::to_string() const {
    switch (m_type) {
        case GameOver::None:
            return "None";
        case GameOver::WinnerWhite:
            return "WinnerWhite";
        case GameOver::WinnerBlack:
            return "WinnerBlack";
        case GameOver::Draw:
            return "Draw";
    }

    assert(false);
    return {};
}

PlayerColor BoardObj::player_color_opponent(PlayerColor color) {
    if (color == PlayerColorWhite) {
        return PlayerColorBlack;
    } else {
        return PlayerColorWhite;
    }
}

const char* BoardObj::player_color_to_string(PlayerColor color) {
    if (color == PlayerColorWhite) {
        return "white";
    } else {
        return "black";
    }
}

void BoardObj::user_click_press() {
    if (m_game_over != GameOver::None) {
        return;
    }

    m_click_id = m_hover_id;
}

void BoardObj::user_click_release(std::function<void()>&& callback) {
    if (m_game_over != GameOver::None) {
        m_click_id = -1;
        return;
    }

    if (m_hover_id == -1 || m_hover_id != m_click_id) {
        m_click_id = -1;
        return;
    }

    m_click_id = -1;

    callback();
}

void BoardObj::update_hover_id(glm::vec3 ray, glm::vec3 camera, std::function<std::vector<HoverableObj>()>&& get_hoverables) {
    if (camera.y < 0.0f) {
        m_hover_id = -1;
        return;
    }

    auto hoverables {get_hoverables()};

    std::sort(hoverables.begin(), hoverables.end(), [camera](const auto& lhs, const auto& rhs) {
        const auto left {glm::distance(lhs.get_renderable().transform.position, camera)};
        const auto right {glm::distance(rhs.get_renderable().transform.position, camera)};

        return left > right;
    });

    bool hover {false};

    for (const HoverableObj& hoverable : hoverables) {
        const auto& transform {hoverable.get_renderable().transform};
        const glm::mat4 to_world_space {transformation_matrix(transform.position, transform.rotation, transform.scale)};

        sm::utils::AABB aabb;
        aabb.min = to_world_space * glm::vec4(hoverable.get_renderable().get_aabb().min, 1.0f);
        aabb.max = to_world_space * glm::vec4(hoverable.get_renderable().get_aabb().max, 1.0f);

        if (ray_aabb_collision(ray, camera, aabb)) {
            m_hover_id = hoverable.get_id();
            hover = true;
        }
    }

    if (!hover) {
        m_hover_id = -1;
    }
}

std::string BoardObj::format(const char* format, ...) {
    std::string buffer;
    buffer.resize(128);

    va_list args;
    va_start(args, format);

    std::vsnprintf(buffer.data(), buffer.size(), format, args);

    va_end(args);

    return buffer;
}
