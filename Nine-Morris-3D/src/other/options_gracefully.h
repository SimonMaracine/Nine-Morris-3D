#pragma once

#include <engine/engine_application.h>
#include <engine/engine_other.h>

#include "other/options.h"

namespace options_gracefully {
    template<typename Opt>
    void save_to_file(std::string_view options_file, const Opt& options) {
        try {
            options::save_options_to_file<Opt>(options, options_file);
        } catch (const options::OptionsFileNotOpenError& e) {
            LOG_DIST_WARNING("Could not save options to file: {}", e.what());

            options::handle_options_file_not_open_error<Opt>(options_file);
        } catch (const options::OptionsFileError& e) {
            LOG_DIST_WARNING("Could not save options to file: {}", e.what());
        }
    }

    template<typename Opt, typename F>
    void load_from_file(std::string_view options_file, Opt& options, const F& validate) {
        try {
            options::load_options_from_file<Opt>(options, options_file, validate);
        } catch (const options::OptionsFileNotOpenError& e) {
            LOG_DIST_WARNING("Could not load options from file: {}", e.what());

            options::handle_options_file_not_open_error<Opt>(options_file);
        } catch (const options::OptionsFileError& e) {
            LOG_DIST_WARNING("Could not load options from file: {}", e.what());

            if (file_system::delete_file(options_file)) {
                LOG_DIST_INFO("Deleted options file `{}`", options_file);
            } else {
                LOG_DIST_ERROR("Could not delete options file `{}`", options_file);
            }

            try {
                options::create_options_file<Opt>(options_file);
            } catch (const options::OptionsFileNotOpenError& e) {
                LOG_DIST_ERROR("Could not create options file: {}", e.what());
            } catch (const options::OptionsFileError& e) {
                LOG_DIST_ERROR("Could not create options file: {}", e.what());
            }
        }
    }
}
