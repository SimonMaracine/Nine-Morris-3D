import OpenGL.GL as GL

from src.engine.log import get_logger

logger = get_logger(__name__)
logger.setLevel(10)


def get_errors():
    while True:
        code = GL.glGetError()
        if code != GL.GL_NO_ERROR:
            logger.error("GL Error: " + str(code))
        else:
            break
