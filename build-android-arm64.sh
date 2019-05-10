#!/bin/sh

TOOLCHAIN=/opt/android-arm64

mkdir -p build-android-arm64
cd build-android-arm64

cmake -DCMAKE_C_COMPILER=$TOOLCHAIN/bin/aarch64-linux-android-clang \
      -DCMAKE_CXX_COMPILER=$TOOLCHAIN/bin/aarch64-linux-android-clang++ \
      -DCMAKE_COLOR_MAKEFILE=1 \
      -DTARGET_ARCH=Android_ARM64 \
      -DCMAKE_BUILD_TYPE=$1 \
      ..

make -j$(nproc)

