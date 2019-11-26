#!/bin/bash

if [ "$1" = "" ]; then
  echo "error: argument dockerfile expected"
  exit 1
fi

if [ ! -f "$1" ]; then
  echo "error: dockerfile $1 not found"
  exit 1
fi

BUILD_DIR=$(dirname $1)
N=$(basename $1)
BUILD_VERSION=$(echo "$N" | cut -d. -f 1)
BUILD_TYPE=$(echo "$N" | cut -d. -f 2)

if [ $BUILD_TYPE = "build" ]; then
  BUILD_NAME="simulavrbuild"
else
  echo "error: unknown build type: $BUILD_TYPE"
  exit 1
fi
  
echo "BUILD_VERSION=$BUILD_VERSION"
echo "BUILD_TYPE=$BUILD_TYPE"
echo "BUILD_DIR=$BUILD_DIR"
echo "BUILD_NAME=$BUILD_NAME"

docker image build -f $1 -t "$BUILD_NAME:$BUILD_VERSION" $BUILD_DIR

# EOF