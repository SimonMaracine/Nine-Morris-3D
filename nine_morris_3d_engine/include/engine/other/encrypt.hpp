#pragma once

#include <string>
#include <utility>
#include <cstddef>
#include <memory>

#include <spdlog/fmt/fmt.h>

#include "engine/application_base/platform.hpp"

#ifdef SM_BUILD_DISTRIBUTION
    #define SM_ENCR(file_path_string) (file_path_string ".dat")
#else
    #define SM_ENCR(file_path_string) (file_path_string)
#endif

namespace cppblowfish {
    class BlowfishContext;
}

namespace sm {
    class Application;

    class EncrFile {
    public:
        explicit EncrFile(const std::string& file_path)
            : file_path(file_path) {}

        const std::string& get() const { return file_path; }
        operator std::string() const { return file_path; }
    private:
        std::string file_path;
    };

    class Encrypt {
    public:
        static std::pair<unsigned char*, std::size_t> load_file(const EncrFile& file_path);

#ifdef SM_BUILD_DISTRIBUTION
        static EncrFile encr(const std::string& file_path) {
            return EncrFile(file_path);
        }
#else
        static std::string encr(const std::string& file_path) {
            return file_path;
        }
#endif
    private:
        static void initialize(const std::string& key);

        static std::unique_ptr<cppblowfish::BlowfishContext> context;

        friend class Application;
    };
}

template<>
struct fmt::formatter<sm::EncrFile> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        if (ctx.begin() != ctx.end()) {
            throw format_error("Invalid format");
        }

        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const sm::EncrFile& file, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", file.get().c_str());  // FIXME false warnings
    }
};
