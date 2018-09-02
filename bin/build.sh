#!/bin/bash

PROJECT_DIR="$(git rev-parse --show-toplevel)"

if [ ! $? -eq 0 ]; then
    echo "For whatever reason, project isn't being build as a git repository. Assuming current directory is the project dir"

    PROJECT_DIR=$(pwd)
fi

BUILD_DIR=$PROJECT_DIR/build
SRC_DIR=$PROJECT_DIR/src
VENDOR_DIR=$PROJECT_DIR/vendor

SDL_FLAGS="$(sdl2-config --cflags --libs)"

mkdir -p $BUILD_DIR

pushd $BUILD_DIR

echo "Building basic SDL2 program..."
g++ -o basic $SRC_DIR/basic.cpp $SDL_FLAGS

echo "Building GLAD..."
gcc -c -o glad.o -I$VENDOR_DIR/glad/include $VENDOR_DIR/glad/src/glad.c

echo "Building game..."
g++ -o game $SRC_DIR/platform_sdl_main.cpp -I$SRC_DIR glad.o -I$VENDOR_DIR/glad/include $SDL_FLAGS -ldl

echo "Done!"

popd
