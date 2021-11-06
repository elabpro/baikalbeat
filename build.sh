#!/bin/sh

DIR=`dirname $0`
cd $DIR || exit 1

echo "* Run cmake:"
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release

echo "* Run make:"
cd build || exit 1
make

echo "* Binary file:"
ls -la baikalbeat
