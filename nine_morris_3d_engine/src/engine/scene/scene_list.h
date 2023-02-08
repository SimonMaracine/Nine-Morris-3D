#pragma once

#include "engine/scene/objects.h"
#include "engine/graphics/gui.h"

class SceneList {
public:
    SceneList() = default;
    ~SceneList() = default;

    SceneList(const SceneList&) = delete;
    SceneList& operator=(const SceneList&) = delete;
    SceneList(SceneList&&) = delete;
    SceneList& operator=(SceneList&&) = delete;

    void add(object::Model* model);
    void remove(object::Model* model);

    void add(object::Quad* quad);
    void remove(object::Quad* quad);

    void add(gui::Image* image);
    void remove(gui::Image* image);

    void add(gui::Text* text);
    void remove(gui::Text* text);

    void clear();
private:
    std::vector<object::Model*> models;
    std::vector<object::Quad*> quads;
    std::vector<gui::Image*> images;
    std::vector<gui::Text*> texts;

    friend class Renderer;
    friend class GuiRenderer;
};
