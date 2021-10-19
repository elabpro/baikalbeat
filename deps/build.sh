#!/bin/bash
# git clone https://github.com/seznam/elasticlient.git
# cd elasticlient
# git submodule update --init --recursive
#cd ..
#git clone https://github.com/mfontanini/cppkafka
#git clone https://github.com/Tencent/rapidjson
#git clone https://github.com/whoshuu/cpr
#mkdir /usr/src/app/cpr/build
mkdir  /usr/src/app/cppkafka/build
#mkdir  /usr/src/app/rapidjson/build
#cd /usr/src/app/cpr/build && cmake .. && make -j4 && make install
#cd /usr/src/app/cppkafka/build && cmake -DCPPKAFKA_BUILD_SHARED=OFF -DCPPKAFKA_BOOST_STATIC_LIBS=ON -DCPPKAFKA_RDKAFKA_STATIC_LIB=ON .. && make -j4 && make install
cd /usr/src/app/cppkafka/build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j4 && make install
#cd /usr/src/app/rapidjson/build && cmake .. && make -j4 && make install
#rm -R /usr/src/app/elasticlient
rm -R /usr/src/app/cppkafka
#rm -R /usr/src/app/rapidjson
