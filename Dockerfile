FROM ubuntu:20.04 as build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y build-essential git gcc cmake libmicrohttpd-dev librdkafka-dev \
    libboost-dev libboost-system-dev libboost-thread-dev libboost-program-options-dev \
    libssl-dev zlib1g-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /usr/src/baikalbeat
WORKDIR /usr/src/baikalbeat

COPY src /usr/src/baikalbeat/src
RUN mkdir -p /usr/src/baikalbeat/build

COPY deps.sh /usr/src/baikalbeat/
COPY build.sh /usr/src/baikalbeat/
COPY clean.sh /usr/src/baikalbeat/
RUN chmod +x /usr/src/baikalbeat/*.sh

RUN /usr/src/baikalbeat/deps.sh
RUN /usr/src/baikalbeat/build.sh

###########################################################

FROM ubuntu:20.04 as prod
RUN apt-get update && \
    apt-get install -y librdkafka1 libboost-system1.71.0 \
    libboost-thread1.71.0 libboost-program-options1.71.0 libssl1.1 zlib1g && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

ENV BB_CONFIG_DIR "/etc/baikalbeat"
ENV BB_CONFIG_FILE "${BB_CONFIG_DIR}/baikalbeat.ini"
ENV BB_BINARY_FILE "/usr/bin/baikalbeat"

COPY --from=build /usr/src/baikalbeat/src/cppkafka/build/src/lib/* /usr/lib/
COPY --from=build /usr/src/baikalbeat/build/baikalbeat "${BB_BINARY_FILE}"
RUN /usr/sbin/ldconfig

RUN mkdir -p "${BB_CONFIG_DIR}"

COPY docker-entrypoint.sh /docker-entrypoint.sh
RUN chmod +x /docker-entrypoint.sh
ENTRYPOINT /docker-entrypoint.sh
