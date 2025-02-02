#include "engines/engine.hpp"

#include <cstring>

void Engine::set_info_callback(std::function<void(const Info&)>&& info_callback) {
    m_info_callback = std::move(info_callback);
}

void Engine::set_log_output(bool enable, const std::filesystem::path& file_path) {
    if (enable) {
        m_log_output_stream.open(file_path, std::ios::app);
    } else {
        m_log_output_stream.close();
    }
}

std::vector<std::string> Engine::parse_message(const std::string& message) {
    std::vector<std::string> tokens;
    std::string buffer {message};

    char* token {std::strtok(buffer.data(), " \t")};

    while (token != nullptr) {
        tokens.emplace_back(token);
        token = std::strtok(nullptr, " \t");
    }

    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const std::string& token) {
        return token.empty();
    }), tokens.end());

    return tokens;
}

std::optional<Engine::Option> Engine::parse_option(const std::vector<std::string>& tokens) {
    Option option;

    const auto name {parse_option_name(tokens)};

    if (!name) {
        return std::nullopt;
    }

    option.name = *name;

    const auto default_ {parse_option_default(tokens)};

    if (!default_) {
        return std::nullopt;
    }

    const auto type {parse_option_type(tokens)};

    if (!type) {
        return std::nullopt;
    }

    if (*type == "check") {
        Option::Check value;

        if (*default_ == "true") {
            value.default_ = true;
        } else if (*default_ == "false") {
            value.default_ = false;
        } else {
            return std::nullopt;
        }

        option.value = value;
    } else if (*type == "spin") {
        Option::Spin value;

        try {
            value.default_ = static_cast<int>(std::stol(*default_));
        } catch (...) {
            return std::nullopt;
        }

        const auto min {parse_option_min(tokens)};

        if (min) {
            value.min = *min;
        }

        const auto max {parse_option_min(tokens)};

        if (max) {
            value.max = *max;
        }

        option.value = value;
    } else if (*type == "combo") {
        Option::Combo value;

        value.default_ = *default_;

        const auto vars {parse_option_vars(tokens)};

        if (vars) {
            value.vars = *vars;
        }

        option.value = value;
    } else if (*type == "string") {
        Option::String value;

        value.default_ = *default_;

        option.value = value;
    } else if (*type == "button") {
        Option::Button value;

        option.value = value;
    }

    return option;
}

std::optional<std::string> Engine::parse_option_name(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "name")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    std::string name;

    while (++iter != tokens.cend()) {
        if (*iter == "type") {
            break;
        }

        name += ' ' + *iter;
    }

    if (name.empty()) {
        return std::nullopt;
    }

    return name.substr(1);
}

std::optional<std::string> Engine::parse_option_type(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "type")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    if (++iter != tokens.cend()) {
        return *iter;
    }

    return std::nullopt;
}

std::optional<std::string> Engine::parse_option_default(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "default")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    std::string default_;

    while (++iter != tokens.cend()) {
        if (*iter == "min" || *iter == "max" || *iter == "var") {
            break;
        }

        default_ += ' ' + *iter;
    }

    if (default_.empty()) {
        return std::nullopt;
    }

    return default_.substr(1);
}

std::optional<int> Engine::parse_option_min(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "min")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    if (++iter != tokens.cend()) {
        try {
            return std::stol(*iter);
        } catch (...) {
            return std::nullopt;
        }
    }

    return std::nullopt;
}

std::optional<int> Engine::parse_option_max(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "max")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    if (++iter != tokens.cend()) {
        try {
            return std::stol(*iter);
        } catch (...) {
            return std::nullopt;
        }
    }

    return std::nullopt;
}

std::optional<std::vector<std::string>> Engine::parse_option_vars(const std::vector<std::string>& tokens) {
    std::vector<std::string>::const_iterator iter {tokens.cbegin()};
    std::vector<std::string> vars;

    while (true) {
        iter = std::find(iter, tokens.cend(), "var");

        if (iter == tokens.cend()) {
            break;
        }

        std::string var;

        while (++iter != tokens.cend()) {
            var += ' ' + *iter;
        }

        if (var.empty()) {
            break;
        }

        vars.push_back(var.substr(1));
    }

    if (vars.empty()) {
        return std::nullopt;
    }

    return vars;
}

bool Engine::token_available(const std::vector<std::string>& tokens, std::size_t index) {
    return index < tokens.size();
}
