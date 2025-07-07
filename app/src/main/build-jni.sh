#!/bin/bash

export NDK_PATH=$ANDROID_SDK_ROOT/ndk/27.0.12077973

rm -rf libs
rm -f libs.zip

$NDK_PATH/ndk-build -C app/src/main/jni

# تحقق من مكان وجود libs
if [ -d "libs" ]; then
  echo "libs found in project root"
  LIBS_DIR=libs
elif [ -d "app/src/main/libs" ]; then
  echo "libs found in app/src/main/libs"
  LIBS_DIR=app/src/main/libs
else
  echo "libs folder not found"
  LIBS_DIR=""
fi

if [ -n "$LIBS_DIR" ] && [ "$(ls -A $LIBS_DIR)" ]; then
  zip -r libs.zip $LIBS_DIR
else
  echo "Warning: libs directory is missing or empty. No zip file created."
fi
