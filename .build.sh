#!/usr/bin/env bash
set -e

export BASE_DIR="$(dirname $(pwd))"

# Install dependencies

sudo apt update
sudo apt install cmake libx11-dev build-essential cmake libavformat-dev libavdevice-dev libswscale-dev libavresample-dev libavutil-dev libmagick++-dev libunittest++-dev swig doxygen libxinerama-dev libxcursor-dev libasound2-dev libxrandr-dev libzmq3-dev git

# Install libopenshot-audio

export PROJECT_NAME="libopenshot-audio"
export NAME_BIN_FOLDER="$PROJECT_NAME-bin"
export INSTALL_PATH="$(dirname $(pwd))/$NAME_BIN_FOLDER"
export GIT_HASH=$(git rev-parse --short HEAD)

mkdir -p $INSTALL_PATH
cd "$BASE_DIR/$PROJECT_NAME"
mkdir -p build/ && cd build/
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH ..
make && make install

cd $INSTALL_PATH
tar -cv . | xz -9 -c - > "$(dirname $(pwd))/$PROJECT_NAME-$GIT_HASH.tar.xz"

cd "$BASE_DIR"
ls
