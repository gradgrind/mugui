#!/bin/sh

rm -rf build/release

mkdir -p build/release

cd build/release

cmake ../..

make
