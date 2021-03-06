#!/bin/bash

# Quick function which exits if the last command ran fails. Intended to be used
# after compiling something.
function compileCheckError() {
    if [ ! $? -eq 0 ]; then
        echo "Compilation failed, exiting."

        exit 1
    fi
}

PROJECT_DIR="$(git rev-parse --show-toplevel)"

if [ ! $? -eq 0 ]; then
    echo "For whatever reason, project isn't being build as a git repository. Assuming current directory is the project dir."

    PROJECT_DIR=$(pwd)
fi

BUILD_DIR=$PROJECT_DIR/build
SRC_DIR=$PROJECT_DIR/src
VENDOR_DIR=$PROJECT_DIR/vendor

SDL_FLAGS="$(sdl2-config --cflags --libs)"
USLIB_FLAGS="-I$VENDOR_DIR/us"

GCC="gcc"
GPP="g++ -Wall -Werror -std=c++11"

START_TIME=$(date +%s)

mkdir -p $BUILD_DIR
pushd $BUILD_DIR

# echo "Building a basic SDL2 program..."
# $GPP -o basic $SRC_DIR/test/basic.cpp $SDL_FLAGS
#
# compileCheckError

echo "Building basic SDL2 audio program..."
$GPP -o audio $SRC_DIR/test/audio.cpp $SDL_FLAGS $USLIB_FLAGS

compileCheckError

echo "Building GLAD..."
$GCC -c -o glad.o -I$VENDOR_DIR/glad/include $VENDOR_DIR/glad/src/glad.c

compileCheckError

echo "Building game..."

$GPP -o game $SRC_DIR/platform_sdl_main.cpp -I$VENDOR_DIR/us -I$SRC_DIR glad.o -I$VENDOR_DIR/glad/include -I$VENDOR_DIR/HandmadeMath $SDL_FLAGS -ldl

compileCheckError

echo "Done!"

END_TIME=$(date +%s)
TOTAL_TIME=$(expr $END_TIME - $START_TIME)

echo "Took to build project: $TOTAL_TIME"

popd
