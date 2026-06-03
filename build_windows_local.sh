#!/bin/bash
CC=x86_64-w64-mingw32-gcc
CFLAGS="-Wall -Wextra -O3 -I. -I../../glfw-3.3.10.bin.WIN64/include -I../../zlib-1.3.1"
LDFLAGS="-L../../glfw-3.3.10.bin.WIN64/lib-mingw-w64 -L../../zlib-1.3.1"
LIBS="-lglfw3 -lopengl32 -lglu32 -lz -lgdi32 -lwinmm -mwindows"

SRC="vec3.c aabb.c tesselator.c textures.c tile.c level.c frustum.c chunk.c level_renderer.c entity.c player.c character.c timer.c input.c font.c mobile_ui.c rubydung.c"

echo "Building RubyDung for Windows..."
$CC $CFLAGS $SRC -o rubydung.exe $LDFLAGS $LIBS
if [ $? -eq 0 ]; then
    echo "Success! rubydung.exe created."
else
    echo "Build failed."
    exit 1
fi
