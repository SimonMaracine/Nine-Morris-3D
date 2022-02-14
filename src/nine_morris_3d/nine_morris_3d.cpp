#include <vector>
#include <memory>

#include "application/icon_image.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/assets.h"
#include "other/logging.h"

// Global reference to application
NineMorris3D* app = nullptr;

NineMorris3D::NineMorris3D()
    : Application(1024, 576, "Nine Morris 3D") {
    try {
        options::load_options_from_file(options);
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        options::handle_options_file_not_open_error();
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file();
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }

    std::vector<std::unique_ptr<IconImage>> icons;
    icons.push_back(std::make_unique<IconImage>(assets::path(assets::ICON_512)));
    icons.push_back(std::make_unique<IconImage>(assets::path(assets::ICON_256)));
    icons.push_back(std::make_unique<IconImage>(assets::path(assets::ICON_128)));
    icons.push_back(std::make_unique<IconImage>(assets::path(assets::ICON_64)));
    icons.push_back(std::make_unique<IconImage>(assets::path(assets::ICON_32)));

    window->set_icons(icons);
}

void NineMorris3D::set_app_pointer(NineMorris3D* instance) {
    assert(app == nullptr);
    app = instance;
}
