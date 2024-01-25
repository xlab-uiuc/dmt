#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

color 13 "Disable THP..."
echo never | sudo tee /sys/kernel/mm/transparent_hugepage/enabled
echo never | sudo tee /sys/kernel/mm/transparent_hugepage/defrag

color 13 "Drop OS caches..."
sudo sync
echo 3 | sudo tee /proc/sys/vm/drop_caches

color 13 "Checking if kernel module is loaded..."

if [ ! -f "/proc/page_tables" ]; then
	color 9 "Kernel module not found, did you run prepare_system.sh?"
	exit 1
fi

color 10 "Everything is OK"
