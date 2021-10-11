FROM ubuntu:20.04 as build

ARG IMAGE_DIR
ARG ARTIFACTS_DIR

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/London

RUN apt-get update && \
    apt-get install -y build-essential git gcc cmake libmicrohttpd-dev librdkafka-dev libboost-dev libboost-system-dev libboost-thread-dev libboost-program-options-dev libssl-dev zlib1g-dev linux-tools-generic linux-tools-$(uname -r) && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

RUN mkdir /usr/src/app/bikalbeat

COPY ${IMAGE_DIR}/deps /usr/src/app/

RUN /usr/src/app/build.sh

COPY ${IMAGE_DIR}/src/* /usr/src/app/baikalbeat/
RUN ldconfig && mkdir /usr/src/app/baikalbeat/build && cd /usr/src/app/baikalbeat/build && cmake .. && make -j4

FROM ubuntu:20.04 as prod
RUN apt-get update && \
    apt-get install -y linux-tools-generic linux-tools-$(uname -r) && \
    apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=build /usr/local/ /usr/local/
COPY --from=build /usr/src/app/baikalbeat/build/baikalbeat /
RUN apt-get update && \
    apt-get install -y libmicrohttpd12 librdkafka1 libboost-system1.71.0 libboost-thread1.71.0 libboost-program-options1.71.0 libssl1.1 zlib1g && \
    apt-get clean && rm -rf /var/lib/apt/lists/*
RUN /usr/sbin/ldconfig
