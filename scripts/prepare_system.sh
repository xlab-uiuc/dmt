#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

# Disable THP
# echo never | sudo tee /sys/kernel/mm/transparent_hugepage/enabled

# Fault all memories
color 13 "Faulting all memories on this machine/VM, this may take a while..."
pushd "$SCRIPT_DIR/../eval"
gcc faultall.c -o faultall
sudo ./faultall || true
popd

# Install kernel module
pushd "$SCRIPT_DIR/../eval/dump_pagetables"
./install.sh
popd

color 13 "Testing kernel module, this may also take a while..."
echo "U1" | sudo tee /proc/page_tables
sudo cat /proc/page_tables > /dev/null

bye
