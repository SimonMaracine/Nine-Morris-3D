from src.model import Model
from src.texture import Texture

board_model = None
board_texture = None
piece_model = None
white_piece_texture = None
black_piece_texture = None


def load():
    global board_model, board_texture, piece_model, white_piece_texture, black_piece_texture
    board_model = Model("data/models/board.obj")
    board_texture = Texture("data/textures/board_texture.png")

    piece_model = Model("data/models/piece.obj")
    white_piece_texture = Texture("data/textures/white_piece_texture.png")
    black_piece_texture = Texture("data/textures/black_piece_texture.png")
