#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

rm -rf libs
rm -f libs.zip

$NDK_PATH/ndk-build -C app/src/main/jni

if [ -d "libs" ] && [ "$(ls -A libs)" ]; then
  zip -r libs.zip libs
else
  echo "Warning: libs directory is missing or empty. No zip file created."
fi
