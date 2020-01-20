import glm
import numpy as np

from OpenGL.GL import *

import src.renderer as renderer
import src.display as display
import src.model as model
import src.errors as errors
import src.camera as camera
from src import events


def main():
    display.init()
    display.create_window()

    renderer.init()
    events.init()
    renderer.set_clear_color(0, 0, 0)

    cam = camera.Camera(glm.vec3(0, 0, -10))

    dragon = model.Model("data/models/dragon.obj")
    # box = model.Model("data/models/box.obj")

    errors.get_errors()

    running = True

    while running:
        for event in events.get_events():
            # print(event)
            if event.type == events.WINDOW_CLOSED:
                running = False

        cam.update()

        renderer.begin(cam)
        renderer.draw(dragon)
        # renderer.draw(box)

        errors.get_errors()

        display.update()

    dragon.dispose()
    renderer.dispose()
    display.dispose()
