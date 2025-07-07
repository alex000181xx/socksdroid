#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

rm -rf libs

$NDK_PATH/ndk-build -C app/src/main/jni
