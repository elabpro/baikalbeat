#!/bin/sh

DIR=`dirname $0`
cd $DIR || exit 1

if [ -z "${IMAGE_NAME}" ]; then
  IMAGE_NAME="docker.horizon.tv/dataops/baikalbeat"
fi

if [ -z "${IMAGE_VERSION}" ]; then
  IMAGE_VERSION="1.9"
fi

BUILD_ARGS="--network host"

if [ -n "${http_proxy}" ]; then
  BUILD_ARGS="${BUILD_ARGS} --build-arg http_proxy=${http_proxy}"
fi

if [ -n "${https_proxy}" ]; then
  BUILD_ARGS="${BUILD_ARGS} --build-arg https_proxy=${https_proxy}"
fi

echo "RUN: docker build . -t ${IMAGE_NAME}:latest ${BUILD_ARGS}"
docker build . -t "${IMAGE_NAME}:latest" ${BUILD_ARGS}
ec="${?}"
if [ "${ec}" -gt "0" ]; then
  echo "ERROR: Docker build failed!"
  exit "${ec}"
fi

echo "RUN: docker push ${IMAGE_NAME}:latest"
docker push "${IMAGE_NAME}:latest"
ec="${?}"
if [ "${ec}" -gt "0" ]; then
  echo "ERROR: Image push failed: ${IMAGE_NAME}:latest"
  exit "${ec}"
fi

if [ -n "${IMAGE_VERSION}" ]; then
  echo "RUN: docker tag ${IMAGE_NAME}:latest ${IMAGE_NAME}:${IMAGE_VERSION}"
  docker tag "${IMAGE_NAME}:latest" "${IMAGE_NAME}:${IMAGE_VERSION}"
  ec="${?}"
  if [ "${ec}" -gt "0" ]; then
    echo "ERROR: Image tag failed: ${IMAGE_NAME}:${IMAGE_VERSION}"
    exit "${ec}"
  fi

  echo "RUN: docker push ${IMAGE_NAME}:${IMAGE_VERSION}"
  docker push "${IMAGE_NAME}:${IMAGE_VERSION}"
  ec="${?}"
  if [ "${ec}" -gt "0" ]; then
    echo "ERROR: Image push failed: ${IMAGE_NAME}:${IMAGE_VERSION}"
    exit "${ec}"
  fi
fi

if [ -n "${IMAGE_VERSION}" -a -n "${GO_PIPELINE_COUNTER}" ]; then
  echo "RUN: docker tag ${IMAGE_NAME}:latest ${IMAGE_NAME}:${IMAGE_VERSION}-${GO_PIPELINE_COUNTER}"
  docker tag "${IMAGE_NAME}:latest" "${IMAGE_NAME}:${IMAGE_VERSION}-${GO_PIPELINE_COUNTER}"
  ec="${?}"
  if [ "${ec}" -gt "0" ]; then
    echo "ERROR: Image tag failed: ${IMAGE_NAME}:${IMAGE_VERSION}-${GO_PIPELINE_COUNTER}"
    exit "${ec}"
  fi

  echo "RUN: docker push ${IMAGE_NAME}:${IMAGE_VERSION}-${GO_PIPELINE_COUNTER}"
  docker push "${IMAGE_NAME}:${IMAGE_VERSION}-${GO_PIPELINE_COUNTER}"
  ec="${?}"
  if [ "${ec}" -gt "0" ]; then
    echo "ERROR: Image push failed: ${IMAGE_NAME}:${IMAGE_VERSION}-${GO_PIPELINE_COUNTER}"
    exit "${ec}"
  fi
fi
