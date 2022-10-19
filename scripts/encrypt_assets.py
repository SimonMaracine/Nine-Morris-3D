import sys
import subprocess

KEY = "data/models/board/board.obj" + "S"
ENGINE_DATA = "../engine_data"
DATA = "../data"
SUFFIX = ".dat"

if sys.platform.startswith("linux"):
    ENCRYPTER = "../encrypter_helper/encrypter"
elif sys.platform.startswith("win32"):
    ENCRYPTER = "../encrypter_helper/encrypter.exe"


def encrypt(file: str):
    subprocess.run([ENCRYPTER, f"{DATA}/{file}", f"{DATA}/{file}{SUFFIX}", KEY])


def encrypt_engine(file: str):
    subprocess.run([ENCRYPTER, f"{ENGINE_DATA}/{file}", f"{ENGINE_DATA}/{file}{SUFFIX}", KEY])


encrypt_engine("shaders/outline.vert")
encrypt_engine("shaders/outline.frag")
encrypt_engine("shaders/quad2d.vert")
encrypt_engine("shaders/quad2d.frag")
encrypt_engine("shaders/quad3d.vert")
encrypt_engine("shaders/quad3d.frag")
encrypt_engine("shaders/screen_quad.vert")
encrypt_engine("shaders/screen_quad.frag")
encrypt_engine("shaders/shadow.vert")
encrypt_engine("shaders/shadow.frag")
encrypt_engine("shaders/skybox.vert")
encrypt_engine("shaders/skybox.frag")
encrypt_engine("shaders/text.vert")
encrypt_engine("shaders/text.frag")

encrypt("cursors/arrow/arrow.png")
encrypt("cursors/cross/cross.png")

encrypt("models/board/board.obj")
encrypt("models/board/board_paint.obj")
encrypt("models/node/node.obj")
encrypt("models/piece/white_piece.obj")
encrypt("models/piece/black_piece.obj")

encrypt("shaders/board/board_paint.vert")
encrypt("shaders/board/board_paint.frag")
encrypt("shaders/board/board.vert")
encrypt("shaders/board/board.frag")
encrypt("shaders/board/board_paint_no_normal.vert")
encrypt("shaders/board/board_paint_no_normal.frag")
encrypt("shaders/board/board_no_normal.vert")
encrypt("shaders/board/board_no_normal.frag")
encrypt("shaders/node/node.vert")
encrypt("shaders/node/node.frag")
encrypt("shaders/piece/piece.vert")
encrypt("shaders/piece/piece.frag")
encrypt("shaders/piece/piece_no_normal.vert")
encrypt("shaders/piece/piece_no_normal.frag")

encrypt("shaders/post_processing/blur.vert")
encrypt("shaders/post_processing/blur.frag")
encrypt("shaders/post_processing/bright_filter.vert")
encrypt("shaders/post_processing/bright_filter.frag")
encrypt("shaders/post_processing/combine.vert")
encrypt("shaders/post_processing/combine.frag")

encrypt("textures/board/board_normal.png")
encrypt("textures/board/board_normal-small.png")
encrypt("textures/board/paint/board_paint.png")
encrypt("textures/board/paint/board_paint-small.png")
encrypt("textures/board/paint/board_paint_labeled.png")
encrypt("textures/board/paint/board_paint_labeled-small.png")
encrypt("textures/board/wood/board_wood.png")
encrypt("textures/board/wood/board_wood-small.png")
encrypt("textures/indicator/white_indicator.png")
encrypt("textures/indicator/black_indicator.png")
encrypt("textures/indicator/wait_indicator.png")
encrypt("textures/indicator/computer_thinking_indicator.png")
encrypt("textures/piece/piece_normal.png")
encrypt("textures/piece/piece_normal-small.png")
encrypt("textures/piece/white/white_piece.png")
encrypt("textures/piece/white/white_piece-small.png")
encrypt("textures/piece/black/black_piece.png")
encrypt("textures/piece/black/black_piece-small.png")
encrypt("textures/skybox/autumn/nx.png")
encrypt("textures/skybox/autumn/nx-small.png")
encrypt("textures/skybox/autumn/ny.png")
encrypt("textures/skybox/autumn/ny-small.png")
encrypt("textures/skybox/autumn/nz.png")
encrypt("textures/skybox/autumn/nz-small.png")
encrypt("textures/skybox/autumn/px.png")
encrypt("textures/skybox/autumn/px-small.png")
encrypt("textures/skybox/autumn/py.png")
encrypt("textures/skybox/autumn/py-small.png")
encrypt("textures/skybox/autumn/pz.png")
encrypt("textures/skybox/autumn/pz-small.png")
encrypt("textures/skybox/field/nx.png")
encrypt("textures/skybox/field/nx-small.png")
encrypt("textures/skybox/field/ny.png")
encrypt("textures/skybox/field/ny-small.png")
encrypt("textures/skybox/field/nz.png")
encrypt("textures/skybox/field/nz-small.png")
encrypt("textures/skybox/field/px.png")
encrypt("textures/skybox/field/px-small.png")
encrypt("textures/skybox/field/py.png")
encrypt("textures/skybox/field/py-small.png")
encrypt("textures/skybox/field/pz.png")
encrypt("textures/skybox/field/pz-small.png")
encrypt("textures/splash_screen/splash_screen.png")
encrypt("textures/splash_screen/launcher/launcher_splash_screen.png")
encrypt("textures/keyboard_controls/keyboard_controls.png")
encrypt("textures/keyboard_controls/keyboard_controls_cross.png")

print("Done")
