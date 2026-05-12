#!/bin/sh

set -e

mkdir -p build-gpp

g++ \
  -std=c++23 \
  -Wall \
  -Wextra \
  -Wpedantic \
  src/*.cpp \
  -o build-gpp/shell

exec ./build-gpp/shell "$@"