#!/bin/bash
#git clone https://github.com/seznam/elasticlient.git
#cd elasticsearch
#git submodule update --init --recursive
#cd ..
#git clone https://github.com/mfontanini/cppkafka
#git clone https://github.com/Tencent/rapidjson
cd /usr/src/app/elasticlient/build && cmake .. && make && make install
cd /usr/src/app/cppkafka/build && cmake .. && make && make install
cd /usr/src/app/rapidjson/build && cmake .. && make && make install
rm -R /usr/src/app/elasticlient
rm -R /usr/src/app/cppkafka
rm -R /usr/src/app/rapidjson
