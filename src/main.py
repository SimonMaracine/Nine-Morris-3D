import glm

# import numpy as np
# from OpenGL.GL import *
from pyglfw.libapi.constant import GLFW_MOUSE_BUTTON_LEFT, GLFW_KEY_ESCAPE

import src.renderer as renderer
import src.display as display
import src.errors as errors
import src.camera as camera
import src.input as input
from src import events
from src.texture import Texture
from src.model import Model
from src.entity import Entity


def main():
    display.init()
    display.create_window()

    renderer.init()
    events.init()
    renderer.set_clear_color(0, 0, 0)

    cam = camera.Camera(glm.vec3(0, 0, -10))

    # dragon_model = Model("data/models/dragon.obj")
    # dragon_texture = Texture("data/textures/whiteish.png")
    # dragon = Entity(dragon_model, dragon_texture)
    #
    # box_model = Model("data/models/box.obj")
    # box_texture = Texture("data/textures/box.png")
    # box = Entity(box_model, box_texture)

    board_model = Model("data/models/board.obj")
    board_texture = Texture("data/textures/board_texture.png")
    board = Entity(board_model, board_texture)

    errors.get_errors()

    running = True

    while running:
        for event in events.get_events():
            if event.type == events.WINDOW_CLOSED:
                running = False
            elif event.type == events.MOUSE_MOVED:
                cam.update_look_direction(event.x_pos, event.y_pos, input.get_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT))
            elif event.type == events.KEY_PRESSED:
                if event.key == GLFW_KEY_ESCAPE:
                    running = False

        cam.update()

        renderer.begin(cam)
        # renderer.draw(dragon, glm.vec3(0), glm.vec3(0), 1)
        # renderer.draw(box, glm.vec3(100, 0, 0), glm.vec3(0), 1.5)
        renderer.draw(board, glm.vec3(0, 0, 0), glm.vec3(0), 1)

        errors.get_errors()

        display.update()

    # dragon.dispose()
    # box.dispose()
    board.dispose()
    renderer.dispose()
    display.dispose()  # Must be last dispose
