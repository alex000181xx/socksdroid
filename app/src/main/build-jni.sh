#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

rm -rf jniLibs
rm -rf libs
rm -rf bin

$NDK_PATH/ndk-build -C app/src/main/jni

mkdir -p jniLibs
mkdir -p bin

for abi in armeabi-v7a arm64-v8a x86 x86_64; do
  mkdir -p jniLibs/$abi
  mkdir -p bin/$abi

  if [ -d "libs/$abi" ]; then
    # نسخ ملفات المكتبات .so فقط إلى jniLibs
    find libs/$abi -maxdepth 1 -name "*.so" -exec cp {} jniLibs/$abi/ \;

    # نسخ الملفات التنفيذية كما هي إلى bin
    cp libs/$abi/tun2socks bin/$abi/ || true
    cp libs/$abi/pdnsd bin/$abi/ || true
  fi
done
