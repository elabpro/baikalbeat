#!/bin/bash
cd /usr/src/app/elasticlient/build && cmake .. && make && make install
cd /usr/src/app/cppkafka/build && cmake .. && make && make install
cd /usr/src/app/rapidjson/build && cmake .. && make && make install
rm -R /usr/src/app/elasticlient
rm -R /usr/src/app/cppkafka
rm -R /usr/src/app/rapidjson
