#include "timer.hpp"

#include <cstdio>

#include "global.hpp"
#include "constants.hpp"

Timer::Timer(std::shared_ptr<sm::Font> font) {
    m_text.font = font;
    m_text.color = glm::vec3(0.75f);
}

void Timer::start() {
    m_running = true;
    m_last_time = sm::Ctx::get_time();
    m_total_time = 0.0;
}

void Timer::stop() {
    m_running = false;
}

void Timer::update() {
    if (!m_running) {
        return;
    }

    const double current_time {sm::Ctx::get_time()};

    m_total_time += current_time - m_last_time;
    m_last_time = current_time;

    while (m_total_time > 0.1) {
        m_total_time -= 0.1;
        m_time++;
    }
}

void Timer::render(sm::Ctx& ctx) {
    const unsigned int time_seconds {m_time / 10};
    const unsigned int minutes {time_seconds / 60};
    const unsigned int seconds {time_seconds % 60};

    char buffer[32] {};
    std::snprintf(buffer, sizeof(buffer), "%.2u:%.2u", minutes, seconds);

    m_text.text = buffer;

    const auto& g {ctx.global<Global>()};

    m_text.scale = (
        sm::utils::map(
           static_cast<float>(ctx.get_window_height()),
            static_cast<float>(MIN_HEIGHT),
            static_cast<float>(MAX_HEIGHT),
            0.5f,
            1.0f
        ) * g.get_scale()
    );

    const auto [width, height] {m_text.font->get_string_size(buffer, m_text.scale)};

    const float offset {40.0f * g.get_scale()};

    m_text.position = glm::vec2(
        static_cast<float>(ctx.get_window_width()) / 2.0f - static_cast<float>(width) / 2.0f,
        static_cast<float>(ctx.get_window_height()) - static_cast<float>(height) - offset
    );

    ctx.add_text(m_text);
}
