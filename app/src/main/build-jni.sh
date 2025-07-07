#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

echo "Contents of JNI source folder before build:"
ls -l app/src/main/jni

rm -rf libs
rm -f libs.zip

$NDK_PATH/ndk-build -C app/src/main/jni

echo "Contents of libs folder after build:"
ls -l libs || echo "libs folder not found"
