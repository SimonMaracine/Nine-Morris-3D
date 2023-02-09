#pragma once

#include "engine/scene/renderables.h"
#include "engine/graphics/gui.h"

class SceneList {
public:
    SceneList() = default;
    ~SceneList() = default;

    SceneList(const SceneList&) = delete;
    SceneList& operator=(const SceneList&) = delete;
    SceneList(SceneList&&) = delete;
    SceneList& operator=(SceneList&&) = delete;

    void add(renderables::Model* model);
    void remove(renderables::Model* model);

    void add(renderables::Quad* quad);
    void remove(renderables::Quad* quad);

    void add(gui::Image* image);
    void remove(gui::Image* image);

    void add(gui::Text* text);
    void remove(gui::Text* text);

    void clear();
private:
    std::vector<renderables::Model*> models;
    std::vector<renderables::Quad*> quads;
    std::vector<gui::Image*> images;
    std::vector<gui::Text*> texts;

    friend class Renderer;
    friend class GuiRenderer;
};
