#!/bin/sh

TOOLCHAIN=/opt/android-arm

mkdir -p build-android-arm
cd build-android-arm

cmake -DCMAKE_C_COMPILER=$TOOLCHAIN/bin/arm-linux-androideabi-clang \
      -DCMAKE_CXX_COMPILER=$TOOLCHAIN/bin/arm-linux-androideabi-clang++ \
      -DCMAKE_COLOR_MAKEFILE=1 \
      -DTARGET_ARCH=Android \
      ..

make

