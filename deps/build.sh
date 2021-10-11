#!/bin/bash
# git clone https://github.com/seznam/elasticlient.git
# cd elasticlient
# git submodule update --init --recursive
#cd ..
#git clone https://github.com/mfontanini/cppkafka
#git clone https://github.com/Tencent/rapidjson
mkdir /usr/src/app/elasticlient/build /usr/src/app/cppkafka/build /usr/src/app/rapidjson/build
cd /usr/src/app/elasticlient/build && cmake .. && make -j4 && make install
cd /usr/src/app/cppkafka/build && cmake .. && make -j4 && make install
cd /usr/src/app/rapidjson/build && cmake .. && make -j4 && make install
rm -R /usr/src/app/elasticlient
rm -R /usr/src/app/cppkafka
rm -R /usr/src/app/rapidjson
