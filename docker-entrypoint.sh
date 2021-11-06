#!/bin/sh
set -e

# set default options
if [ -z "${1}" ]; then
  set -- "-config ${BB_CONFIG_FILE}"
fi

# first arg is `-f` or `--some-option`
if [ "${1#-}" != "$1" ]; then
  set -- "${BB_BINARY_FILE}" "$@"
fi

env | grep '^BB_'
echo RUN: $@
exec $@
