import os

DATA_FILE = "assets.morrisdat"
ASSETS_LIST = (
    "data/fonts/OpenSans-Semibold.ttf",
    "data/models/board.obj",
    "data/shaders/board.vert",
    "data/shaders/board.frag",
    "data/shaders/cubemap.vert",
    "data/shaders/cubemap.frag",
    "data/shaders/light.vert",
    "data/shaders/light.frag",
    "data/shaders/loading.vert",
    "data/shaders/loading.frag",
    "data/shaders/node.vert",
    "data/shaders/node.frag",
    "data/shaders/origin.vert",
    "data/shaders/origin.frag",
    "data/shaders/outline.vert",
    "data/shaders/outline.frag",
    "data/shaders/piece.vert",
    "data/shaders/piece.frag",
    "data/shaders/quad.vert",
    "data/shaders/quad.frag",
    "data/shaders/shadow.vert",
    "data/shaders/shadow.frag",
    "data/textures/skybox/back.jpg",
    "data/textures/skybox/bottom.jpg",
    "data/textures/skybox/front.jpg",
    "data/textures/skybox/left.jpg",
    "data/textures/skybox/right.jpg",
    "data/textures/skybox/top.jpg",
    "data/textures/black_piece.png",
    "data/textures/white_piece.png",
    "data/textures/board.png",
    "data/textures/light.png",
    "data/textures/loading.png"
)
HEAD_SIZE = len(ASSETS_LIST) * 8  # In bytes, use 8 bytes integer
SEPARATOR_SIZE = 4
SEPARATOR = "ssss"


def make_data_file():
    offsets = []
    current_offset = 0

    with open(DATA_FILE, "wb") as data_file:
        data_file.seek(HEAD_SIZE + SEPARATOR_SIZE)

        for file in ASSETS_LIST:
            with open(file, "rb") as f:
                data = f.read()
                data_file.write(data)

                current_offset += len(data)
                offsets.append(current_offset)

        data_file.seek(0)

        for offset in offsets:
            data_file.write(offset.to_bytes(8, "big"))

        data_file.write(bytes(SEPARATOR, encoding="ascii"))


def main():
    os.chdir("..")
    make_data_file()
    print("Done packaging assets")


if __name__ == "__main__":
    main()
