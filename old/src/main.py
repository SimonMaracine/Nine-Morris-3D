import glm
import pyglfw.libapi as GLFW

from src.engine import renderer
from src.engine import display
from src.engine import events
from src.engine import input
from src.engine import errors
from src.engine.camera import Camera
from src.engine.mouse_ray import MouseRay
from src.game.board import Board


def main():
    display.init()
    renderer.init()
    events.init()

    renderer.set_clear_color(0.2, 0.2, 0.6)

    camera = Camera(glm.vec3(-7, 7.5, -0.3), glm.vec3())

    mouse_ray = MouseRay(camera, renderer.proj_matrix)
    board = Board()

    errors.get_errors()

    running = True

    while running:
        for event in events.get_events():
            if event.type == events.EventType.WINDOW_CLOSED:
                running = False
            elif event.type == events.EventType.MOUSE_MOVED:
                camera.update_look_direction()
            elif event.type == events.EventType.KEY_PRESSED:
                if event.key == GLFW.GLFW_KEY_ESCAPE:
                    running = False

        camera.update()
        mouse_ray.update()
        board.update(mouse_ray.current_ray)
        # print(mouse_ray.current_ray)

        renderer.begin(camera)
        board.render()
        errors.get_errors()

        display.update()

    board.dispose()
    renderer.dispose()
    display.dispose()  # Must be last dispose
