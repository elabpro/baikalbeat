#!/bin/sh

DIR=`dirname $0`
cd $DIR || exit 1

echo "* Clean"
cd build || exit 1
rm -rvf *
