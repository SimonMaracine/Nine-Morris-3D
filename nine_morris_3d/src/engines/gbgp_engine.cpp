#include "gbgp_engine.hpp"

#include <chrono>
#include <algorithm>
#include <numeric>
#include <utility>

using namespace std::string_literals;
using namespace std::chrono_literals;

void GbgpEngine::initialize(const std::string& file_path) {
    try {
        m_subprocess.open(file_path);
    } catch (const SubprocessError& e) {
        throw EngineError("Could not start subprocess: "s + e.what());
    }

    try {
        m_subprocess.write_line("gbgp");
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }

    const auto begin {std::chrono::steady_clock::now()};

    while (true) {
        const auto now {std::chrono::steady_clock::now()};

        if (now - begin > 5s) {
            throw EngineError("Engine did not respond in a timely manner");
        }

        std::string message;

        try {
            message = m_subprocess.read_line();
        } catch (const SubprocessError& e) {
            throw EngineError("Could not read from subprocess: "s + e.what());
        }

        if (message.empty()) {
            continue;
        }

        if (m_log_output_stream.is_open()) {
            m_log_output_stream << message << '\n';
            m_log_output_stream.flush();
        }

        const auto tokens {parse_message(message)};

        if (tokens.empty()) {
            continue;
        }

        if (tokens[0] == "gbgpok") {
            break;
        } else if (tokens[0] == "id") {
            if (token_available(tokens, 1)) {
                if (tokens[1] == "name") {
                    std::size_t index {2};
                    while (token_available(tokens, index)) {
                        m_name += ' ' + tokens[index++];
                    }
                    m_name = m_name.substr(1);
                } else if (tokens[1] == "author") {
                    std::size_t index {2};
                    while (token_available(tokens, index)) {
                        m_author += ' ' + tokens[index++];
                    }
                    m_author = m_author.substr(1);
                }
            }
        } else if (tokens[0] == "option") {
            const auto option {parse_option(tokens)};

            if (option) {
                m_options.push_back(*option);
            }
        }
    }
}

void GbgpEngine::set_debug(bool active) {
    try {
        m_subprocess.write_line("debug"s + (active ? " on" : " off"));
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }
}

void GbgpEngine::synchronize() {
    try {
        m_subprocess.write_line("isready");
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }

    const auto begin {std::chrono::steady_clock::now()};

    while (true) {
        const auto now {std::chrono::steady_clock::now()};

        if (now - begin > 5s) {
            throw EngineError("Engine did not respond in a timely manner");
        }

        std::string message;

        try {
            message = m_subprocess.read_line();
        } catch (const SubprocessError& e) {
            throw EngineError("Could not read from subprocess: "s + e.what());
        }

        if (message.empty()) {
            continue;
        }

        if (m_log_output_stream.is_open()) {
            m_log_output_stream << message << '\n';
            m_log_output_stream.flush();
        }

        const auto tokens {parse_message(message)};

        if (tokens.empty()) {
            continue;
        }

        if (tokens[0] == "readyok") {
            break;
        }
    }
}

void GbgpEngine::set_option(const std::string& name, const std::optional<std::string>& value) {
    try {
        m_subprocess.write_line("setoption name " + name + (value ? " value " + *value : ""));
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }
}

void GbgpEngine::new_game() {
    try {
        m_subprocess.write_line("newgame");
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }
}

void GbgpEngine::start_thinking(
    const std::optional<std::string>& position,
    const std::vector<std::string>& moves,
    std::optional<unsigned int> wtime,
    std::optional<unsigned int> btime,
    std::optional<unsigned int> movetime
) {
    const auto moves_str {
        !moves.empty()
        ?
        " moves " + std::accumulate(++moves.cbegin(), moves.cend(), *moves.cbegin(), [](std::string r, const std::string& move) {
            return std::move(r) + " " + move;
        })
        :
        ""
    };

    try {
        m_subprocess.write_line("position" + (position ? " pos " + *position : " startpos") + moves_str);
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }

    try {
        m_subprocess.write_line(
            "go"s +
            (wtime ? " wtime " + std::to_string(*wtime) : "") +
            (btime ? " btime " + std::to_string(*btime) : "") +
            (movetime ? " movetime " + std::to_string(*movetime) : "")
        );
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }
}

void GbgpEngine::stop_thinking() {
    try {
        m_subprocess.write_line("stop");
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }
}

std::optional<std::string> GbgpEngine::done_thinking() {
    std::string message;

    try {
        message = m_subprocess.read_line();
    } catch (const SubprocessError& e) {
        throw EngineError("Could not read from subprocess: "s + e.what());
    }

    if (message.empty()) {
        return std::nullopt;
    }

    if (m_log_output_stream.is_open()) {
        m_log_output_stream << message << '\n';
        m_log_output_stream.flush();
    }

    const auto tokens {parse_message(message)};

    if (tokens.empty()) {
        return std::nullopt;
    }

    if (tokens[0] == "bestmove") {
        if (token_available(tokens, 1)) {
            return tokens[1];
        }
    } else if (tokens[0] == "info") {
        if (m_info_callback) {
            m_info_callback(parse_info(tokens));
        }
    }

    return std::nullopt;
}

void GbgpEngine::uninitialize() {
    m_name.clear();

    try {
        m_subprocess.write_line("quit");
    } catch (const SubprocessError& e) {
        throw EngineError("Could not write to subprocess: "s + e.what());
    }

    try {
        m_subprocess.wait();
    } catch (const SubprocessError& e) {
        throw EngineError("Could not wait for subprocess: "s + e.what());
    }
}

bool GbgpEngine::is_null_move(const std::string& move) const {
    return move == "none";
}

GbgpEngine::Info GbgpEngine::parse_info(const std::vector<std::string>& tokens) {
    Info info;
    info.depth = parse_info_ui(tokens, "depth");
    info.time = parse_info_ui(tokens, "time");
    info.nodes = parse_info_ui(tokens, "nodes");
    info.score = parse_info_score(tokens);
    info.pv = parse_info_pv(tokens);

    return info;
}

std::optional<unsigned int> GbgpEngine::parse_info_ui(const std::vector<std::string>& tokens, const std::string& name) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), name)};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    if (++iter != tokens.cend()) {
        try {
            return std::stoul(*iter);
        } catch (...) {
            return std::nullopt;
        }
    }

    return std::nullopt;
}

std::optional<GbgpEngine::Info::Score> GbgpEngine::parse_info_score(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "score")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    if (++iter == tokens.cend()) {
        return std::nullopt;
    }

    if (*iter == "eval") {
        if (++iter != tokens.cend()) {
            try {
                return Info::ScoreEval {std::stoi(*iter)};
            } catch (...) {
                return std::nullopt;
            }
        }
    } else if (*iter == "win") {
        if (++iter != tokens.cend()) {
            try {
                return Info::ScoreWin {std::stoi(*iter)};
            } catch (...) {
                return std::nullopt;
            }
        }
    }

    return std::nullopt;
}

std::optional<std::vector<std::string>> GbgpEngine::parse_info_pv(const std::vector<std::string>& tokens) {
    auto iter {std::find(tokens.cbegin(), tokens.cend(), "pv")};

    if (iter == tokens.cend()) {
        return std::nullopt;
    }

    std::vector<std::string> pv;

    while (++iter != tokens.cend()) {
        pv.push_back(*iter);
    }

    return pv;
}
