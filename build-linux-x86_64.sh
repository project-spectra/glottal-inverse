#!/bin/sh

#TOOLCHAIN=/opt/android-arm64

mkdir -p build-linux-x86_64
cd build-linux-x86_64

cmake -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_COLOR_MAKEFILE=1 \
      -DTARGET_ARCH=Linux \
      -DCMAKE_BUILD_TYPE=$1 \
      ..

make -j$(nproc)

