#pragma once

#include <resmanager/resmanager.h>

#include "engine/graphics/actors.h"
#include "engine/graphics/gui.h"

struct SceneList {
    resmanager::Cache<Model> model;
    resmanager::Cache<Quad> quad;
    resmanager::Cache<gui::Image> image;
    resmanager::Cache<gui::Text> text;
};
