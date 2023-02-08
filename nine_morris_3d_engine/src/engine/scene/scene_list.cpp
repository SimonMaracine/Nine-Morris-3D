#include "engine/graphics/gui.h"
#include "engine/other/logging.h"
#include "engine/scene/scene_list.h"
#include "engine/scene/objects.h"

void SceneList::add(object::Model* model) {
    const auto iter = std::find(models.cbegin(), models.cend(), model);

    if (iter != models.cend()) {
        DEB_WARNING("Model already present in list");
        return;
    }

    models.push_back(model);
}

void SceneList::remove(object::Model* model) {
    const auto iter = std::find(models.cbegin(), models.cend(), model);

    if (iter == models.cend()) {
        return;
    }

    models.erase(iter);
}

void SceneList::add(object::Quad* quad) {
    const auto iter = std::find(quads.cbegin(), quads.cend(), quad);

    if (iter != quads.cend()) {
        DEB_WARNING("Quad already present in list");
        return;
    }

    quads.push_back(quad);
}

void SceneList::remove(object::Quad* quad) {
    const auto iter = std::find(quads.cbegin(), quads.cend(), quad);

    if (iter == quads.cend()) {
        return;
    }

    quads.erase(iter);
}

void SceneList::add(gui::Image* image) {
    const auto iter = std::find(images.cbegin(), images.cend(), image);

    if (iter != images.cend()) {
        DEB_WARNING("Widget already present");
        return;
    }

    images.push_back(image);
}

void SceneList::remove(gui::Image* image) {
    const auto iter = std::find(images.cbegin(), images.cend(), image);

    if (iter == images.cend()) {
        return;
    }

    images.erase(iter);
}

void SceneList::add(gui::Text* text) {
    const auto iter = std::find(texts.cbegin(), texts.cend(), text);

    if (iter != texts.cend()) {
        DEB_WARNING("Widget already present");
        return;
    }

    texts.push_back(text);
}

void SceneList::remove(gui::Text* text) {
    const auto iter = std::find(texts.cbegin(), texts.cend(), text);

    if (iter == texts.cend()) {
        return;
    }

    texts.erase(iter);
}

void SceneList::clear() {
    models.clear();
    quads.clear();
    images.clear();
    texts.clear();
}
