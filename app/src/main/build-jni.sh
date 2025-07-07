#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

rm -rf jniLibs
rm -rf libs

$NDK_PATH/ndk-build -C app/src/main/jni

for p in armeabi-v7a arm64-v8a x86 x86_64; do
  mv libs/$p/tun2socks libs/$p/libtun2socks.so
  mv libs/$p/pdnsd libs/$p/libpdnsd.so
done

mv libs jniLibs
