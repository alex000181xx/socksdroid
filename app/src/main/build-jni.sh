#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

rm -rf app/src/main/jniLibs
rm -rf libs
rm -rf app/src/main/bin

$NDK_PATH/ndk-build -C app/src/main/jni

mkdir -p app/src/main/jniLibs
mkdir -p app/src/main/bin

for abi in armeabi-v7a arm64-v8a x86 x86_64; do
  mkdir -p app/src/main/jniLibs/$abi
  mkdir -p app/src/main/bin/$abi

  if [ -d "libs/$abi" ]; then
    # نسخ ملفات المكتبات .so فقط إلى jniLibs
    find libs/$abi -maxdepth 1 -name "*.so" -exec cp {} app/src/main/jniLibs/$abi/ \;

    # نسخ الملفات التنفيذية كما هي إلى bin
    cp libs/$abi/tun2socks app/src/main/bin/$abi/ || true
    cp libs/$abi/pdnsd app/src/main/bin/$abi/ || true
  fi
done
