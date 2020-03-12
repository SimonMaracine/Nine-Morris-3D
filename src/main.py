import glm

# import numpy as np
# from OpenGL.GL import *
from pyglfw.libapi.constant import GLFW_MOUSE_BUTTON_LEFT, GLFW_KEY_ESCAPE

import src.renderer as renderer
import src.display as display
import src.errors as errors
import src.camera as camera
import src.input as input
import src.assets as assets
from src.board import Board
from src.mouse_ray import MouseRay
from src import events


def main():
    display.init()
    display.create_window()
    assets.load()

    renderer.init()
    events.init()
    renderer.set_clear_color(0.2, 0.2, 0.6)

    cam = camera.Camera(glm.vec3(0, 0, -10))

    # dragon_model = Model("data/models/dragon.obj")
    # dragon_texture = Texture("data/textures/whiteish.png")
    # dragon = Entity(dragon_model, dragon_texture)
    #
    # box_model = Model("data/models/box.obj")
    # box_texture = Texture("data/textures/box.png")
    # box = Entity(box_model, box_texture)

    ray = MouseRay(cam, renderer.proj_matrix)

    board = Board()

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
        ray.update()

        # print(ray.current_ray)

        renderer.begin(cam)
        # renderer.draw(dragon, glm.vec3(0), glm.vec3(0), 1)
        # renderer.draw(box, glm.vec3(100, 0, 0), glm.vec3(0), 1.5)
        board.render()
        # renderer.draw(white_piece, PIECES_ORIGIN + glm.vec3(PIECES_GRID, 0, PIECES_GRID), glm.vec3(0), 0.2)
        # renderer.draw(black_piece, PIECES_ORIGIN + glm.vec3(0, 0, PIECES_GRID * 3), glm.vec3(0), 0.2)
        errors.get_errors()

        display.update()

    # dragon.dispose()
    # box.dispose()
    renderer.dispose()
    display.dispose()  # Must be last dispose
