#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../scripts/common.sh"

mkdir -p build
pushd build

cmake ..
make CC=gcc-7 CXX=g++-7 -j

popd

bye
