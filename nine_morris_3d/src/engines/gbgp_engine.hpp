#pragma once

#include <variant>

#include "engines/engine.hpp"

class GbgpEngine : public Engine {
public:
    struct Info : Engine::Info {
        struct ScoreEval { int value; };
        struct ScoreWin { int value; };

        using Score = std::variant<ScoreEval, ScoreWin>;

        std::optional<unsigned int> depth;
        std::optional<unsigned int> time;
        std::optional<unsigned int> nodes;
        std::optional<Score> score;
        std::optional<std::vector<std::string>> pv;
    };

    void initialize(const std::filesystem::path& file_path, bool search_executable = false) override;
    void set_debug(bool active) override;
    void synchronize() override;
    void set_option(const std::string& name, const std::optional<std::string>& value) override;
    void new_game() override;
    void start_thinking(
        const std::optional<std::string>& position,
        const std::vector<std::string>& moves,
        std::optional<unsigned int> wtime,
        std::optional<unsigned int> btime,
        std::optional<unsigned int> movetime
    ) override;
    void stop_thinking() override;
    std::optional<std::string> done_thinking() override;
    void uninitialize() override;

    bool is_null_move(const std::string& move) const override;
private:
    static Info parse_info(const std::vector<std::string>& tokens);
    static std::optional<unsigned int> parse_info_ui(const std::vector<std::string>& tokens, const std::string& name);
    static std::optional<Info::Score> parse_info_score(const std::vector<std::string>& tokens);
    static std::optional<std::vector<std::string>> parse_info_pv(const std::vector<std::string>& tokens);
};
