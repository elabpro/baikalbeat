#!/bin/sh

DIR=`dirname $0`
cd $DIR || exit 1

cd src || exit 1

echo "* Clone: cppkafka"
if [ ! -d 'cppkafka' ]; then
  git clone "https://github.com/mfontanini/cppkafka"
fi

#echo "* Cmake: cppkafka"
#cd cppkafka || exit 1
#mkdir -p build
#cmake -S . -B build
#
#echo "* Make: cppkafka"
#cd build || exit 1
#make

#if [ ! -d 'spdlog' ]; then
#  git clone "https://github.com/gabime/spdlog"
#fi
#
#if [ ! -d 'popl' ]; then
#  git clone "https://github.com/badaix/popl"
#fi
