#include "muhle_engine.hpp"

#include <cstring>
#include <cassert>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <muhle_intelligence.h>

namespace muhle_engine {
    void initialize() {
        if (muhle_intelligence_initialize() == MUHLE_INTELLIGENCE_ERROR) {
            LOG_DIST_ERROR("Could not initialize engine");
        }
    }

    void uninitialize() {
        if (muhle_intelligence_uninitialize() == MUHLE_INTELLIGENCE_ERROR) {
            LOG_DIST_ERROR("Could not uninitialize engine");
        }
    }

    std::optional<std::string> receive_message() {
        unsigned int size {};
        const int result {muhle_intelligence_receive_size(&size)};

        if (result == MUHLE_INTELLIGENCE_ERROR) {
            LOG_DIST_ERROR("Could not receive message size");
            return std::nullopt;
        }

        if (result == MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE) {
            return std::nullopt;
        }

        {
            std::string buffer;
            buffer.resize(size);

            const int result {muhle_intelligence_receive(buffer.data())};

            if (result == MUHLE_INTELLIGENCE_ERROR) {
                LOG_DIST_ERROR("Could not receive message");
                return std::nullopt;
            }

            assert(result == MUHLE_INTELLIGENCE_SUCCESS);

            return std::make_optional(buffer);
        }
    }

    void send_message(const std::string& message) {
        if (muhle_intelligence_send(message.c_str()) == MUHLE_INTELLIGENCE_ERROR) {
            LOG_DIST_ERROR("Could not send message");
        }
    }

    bool is_ready() {
        const auto message {receive_message()};

        if (!message) {
            return false;
        }

        if (*message == "ready") {
            return true;
        }

        return false;
    }

    std::vector<std::string> tokenize_message(const std::string& message) {
        std::vector<std::string> tokens;

        std::string mutable_buffer {message};

        char* token {std::strtok(mutable_buffer.data(), " \t")};

        while (token != nullptr) {
            tokens.emplace_back(token);

            token = std::strtok(nullptr, " \t");
        }

        // The last token always has an unwanted new line
        tokens.back() = tokens.back().substr(0, tokens.back().size() - 1);

        return tokens;
    }
}
