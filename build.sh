#!/bin/bash

PROJECT_DIR="$(git rev-parse --show-toplevel)"

if [ ! $? -eq 0 ]; then
    echo "For whatever reason, project isn't being build as a git repository. Assuming current directory is the project dir"

    PROJECT_DIR=$(pwd)
fi

BUILD_DIR=$PROJECT_DIR/build

SRC_DIR=$PROJECT_DIR/src

mkdir -p $BUILD_DIR

pushd $BUILD_DIR

g++ -o basic $SRC_DIR/basic.cpp `sdl2-config --cflags --libs`

popd
