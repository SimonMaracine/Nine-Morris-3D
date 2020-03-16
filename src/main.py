import glm

from pyglfw.libapi.constant import GLFW_KEY_ESCAPE

from engine import renderer, display, events, input, errors, Camera, MouseRay
from game.board import Board


def main():
    renderer.set_clear_color(0.2, 0.2, 0.6)

    cam = Camera(glm.vec3(-7, 7.5, -0.3), glm.vec3())

    ray = MouseRay(cam, renderer.proj_matrix)
    board = Board()

    errors.get_errors()

    running = True

    while running:
        for event in events.get_events():
            if event.type == events.WINDOW_CLOSED:
                running = False
            elif event.type == events.MOUSE_MOVED:
                cam.update_look_direction()
            elif event.type == events.KEY_PRESSED:
                if event.key == GLFW_KEY_ESCAPE:
                    running = False

        cam.update()
        ray.update()
        board.update(ray.current_ray)

        # print(ray.current_ray)

        renderer.begin(cam)
        board.render()
        errors.get_errors()

        display.update()

    # dragon.dispose()
    # box.dispose()
    renderer.dispose()
    display.dispose()  # Must be last dispose
