#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/23.1.7779620

LIBS_DIR=libs
rm -rf $LIBS_DIR
rm -f libs.zip

$NDK_PATH/ndk-build -C app/src/main/jni

# تحقق من مكان وجود libs فعليًا
if [ -d "libs" ]; then
  echo "libs found in project root"
  LIBS_DIR=libs
elif [ -d "app/src/main/libs" ]; then
  echo "libs found in app/src/main/libs"
  LIBS_DIR=app/src/main/libs
else
  echo "libs folder not found"
fi

if [ -d "$LIBS_DIR" ] && [ "$(ls -A $LIBS_DIR)" ]; then
  zip -r libs.zip $LIBS_DIR
else
  echo "Warning: libs directory is missing or empty. No zip file created."
fi
