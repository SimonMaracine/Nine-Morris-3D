from OpenGL.GL import *


def get_errors():
    code: int = glGetError()
    while code != GL_NO_ERROR:
        print("GL Error: " + str(code))
