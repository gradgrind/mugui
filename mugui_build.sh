#!/bin/sh

rm -rf build/release

mkdir -p build/release

if [ ! -d build/examples ]; then
    ln -s ../examples build/examples
fi

cd build/release

cmake ../..

make
