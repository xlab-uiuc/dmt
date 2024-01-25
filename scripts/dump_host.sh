#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

if [ $# -ne 1 ]; then
	color 9 "$0 [pid_of_hypervisor]"
	exit 1
fi

pid="$1"

color 13 "Dumping hypervisor page tables"

echo "V$pid" | sudo tee /proc/page_tables
sleep 5
sudo cat /proc/page_tables > "$SCRIPT_DIR/pt_dump.host"

color 13 "Processing page table data"
"$SCRIPT_DIR/../eval/transform_pt_host.pl" "$SCRIPT_DIR/pt_dump.host"

if [ -f "pt_dump.host" ]; then
	echo "The page table dump is ready at $(realpath -e pt_dump.host)"
else
	false
fi

bye
