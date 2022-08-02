#!/bin/sh

set -x

BUILD_DIR=build

if [[ ! -d $BUILD_DIR ]]; then
	mkdir -p $BUILD_DIR
fi

cd $BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
cmake --build .
ctest
