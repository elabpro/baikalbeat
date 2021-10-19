FROM ubuntu:20.04 as prod
RUN apt-get update && \
    apt-get install -y librdkafka1 libboost-system1.71.0 \
    libboost-thread1.71.0 libboost-program-options1.71.0 libssl1.1 zlib1g && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

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
RUN rm -f /usr/src/app/baikalbeat/CMakeCache.txt && ldconfig && cd /usr/src/app/baikalbeat && cmake -DCMAKE_BUILD_TYPE=Release . && make -j4

FROM prod as prod-final
COPY --from=build /usr/local/lib/ /usr/local/lib/
COPY --from=build /usr/src/app/baikalbeat/baikalbeat /
RUN /usr/sbin/ldconfig
