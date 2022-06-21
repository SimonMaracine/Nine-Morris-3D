#! /bin/bash

KEY="data/models/board/board.objS"
ENCRYPTER="../encrypter-helper/encrypter"
DATA="../data"
SUFFIX=".dat"

function encrypt() {
    $ENCRYPTER $DATA/$1 $DATA/$1$SUFFIX $KEY
}

encrypt cursors/arrow.png
encrypt cursors/cross.png

encrypt models/board/board.obj
encrypt models/board/board_paint.obj
encrypt models/node/node.obj
encrypt models/piece/white_piece.obj
encrypt models/piece/black_piece.obj

encrypt shaders/internal/origin.vert
encrypt shaders/internal/origin.frag
encrypt shaders/internal/outline.vert
encrypt shaders/internal/outline.frag
encrypt shaders/internal/quad2d.vert
encrypt shaders/internal/quad2d.frag
encrypt shaders/internal/quad3d.vert
encrypt shaders/internal/quad3d.frag
encrypt shaders/internal/screen_quad.vert
encrypt shaders/internal/screen_quad.frag
encrypt shaders/internal/shadow.vert
encrypt shaders/internal/shadow.frag
encrypt shaders/internal/skybox.vert
encrypt shaders/internal/skybox.frag
encrypt shaders/internal/text.vert
encrypt shaders/internal/text.frag
encrypt shaders/board_paint.vert
encrypt shaders/board_paint.frag
encrypt shaders/board.vert
encrypt shaders/board.frag
encrypt shaders/node.vert
encrypt shaders/node.frag
encrypt shaders/piece.vert
encrypt shaders/piece.frag

encrypt textures/board/board_normal.png
encrypt textures/board/board_paint.png
encrypt textures/board/board_paint-small.png
encrypt textures/board/board_wood.png
encrypt textures/board/board_wood-small.png
encrypt textures/indicator/white_indicator.png
encrypt textures/indicator/black_indicator.png
encrypt textures/piece/piece_normal.png
encrypt textures/piece/white_piece.png
encrypt textures/piece/white_piece-small.png
encrypt textures/piece/black_piece.png
encrypt textures/piece/black_piece-small.png
encrypt textures/skybox/autumn/nx.png
encrypt textures/skybox/autumn/nx-small.png
encrypt textures/skybox/autumn/ny.png
encrypt textures/skybox/autumn/ny-small.png
encrypt textures/skybox/autumn/nz.png
encrypt textures/skybox/autumn/nz-small.png
encrypt textures/skybox/autumn/px.png
encrypt textures/skybox/autumn/px-small.png
encrypt textures/skybox/autumn/py.png
encrypt textures/skybox/autumn/py-small.png
encrypt textures/skybox/autumn/pz.png
encrypt textures/skybox/autumn/pz-small.png
encrypt textures/skybox/field/nx.png
encrypt textures/skybox/field/nx-small.png
encrypt textures/skybox/field/ny.png
encrypt textures/skybox/field/ny-small.png
encrypt textures/skybox/field/nz.png
encrypt textures/skybox/field/nz-small.png
encrypt textures/skybox/field/px.png
encrypt textures/skybox/field/px-small.png
encrypt textures/skybox/field/py.png
encrypt textures/skybox/field/py-small.png
encrypt textures/skybox/field/pz.png
encrypt textures/skybox/field/pz-small.png
encrypt textures/splash_screen/splash_screen.png

echo "Done"
