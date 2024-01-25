#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

. "$SCRIPT_DIR/workloads.sh"

cd "$SCRIPT_DIR/../processor"

for work in "${WORKLOADS[@]}"; do
	color 13 "---------------------------------------------------------"
	color 13 "Result for workload $work in NPT or SPT virtualized case"
	color 13 ""
	sudo dotnet run -- "$SCRIPT_DIR/../data/" "$work" nested "$ept" "$spt"
	color 13 ""
done

bye
