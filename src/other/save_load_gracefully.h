#pragma once

#include <nine_morris_3d_engine/engine_application.h>
#include <nine_morris_3d_engine/engine_other.h>

#include "other/options.h"

namespace save_load_gracefully {
    template<typename Opt>
    void save_to_file(std::string_view options_file, const Opt& options, Application* app) {
        try {
            options::save_options_to_file<Opt>(options, options_file);
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("Could not save options to file: {}", e.what());

            options::handle_options_file_not_open_error<Opt>(
                options_file, app->data().application_name
            );
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("Could not save options to file: {}", e.what());

            try {
                options::create_options_file<Opt>(options_file);
            } catch (const options::OptionsFileNotOpenError& e) {
                REL_ERROR("Could not create options file: {}", e.what());
            } catch (const options::OptionsFileError& e) {
                REL_ERROR("Could not create options file: {}", e.what());
            }
        }
    }

    template<typename Opt, typename F>
    void load_from_file(std::string_view options_file, Opt& options, const F& validate, Application* app) {
        try {
            options::load_options_from_file<Opt>(options, options_file, validate);
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("Could not load options to file: {}", e.what());

            options::handle_options_file_not_open_error<Opt>(
                options_file, app->data().application_name
            );
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("Could not load options to file: {}", e.what());

            try {
                options::create_options_file<Opt>(options_file);
            } catch (const options::OptionsFileNotOpenError& e) {
                REL_ERROR("Could not create options file: {}", e.what());
            } catch (const options::OptionsFileError& e) {
                REL_ERROR("Could not create options file: {}", e.what());
            }
        }
    }
}
