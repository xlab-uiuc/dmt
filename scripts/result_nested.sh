#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

. "$SCRIPT_DIR/workloads.sh"

cd "$SCRIPT_DIR/../processor"

function read_num {
	while :; do
		read -r -p "$2 " ans

		if echo "$ans" | perl -ne 'exit 1 unless(/^\d+(\.\d+)?$/gm)'; then
			eval "$1=$ans"
			break
		fi

		echo "Please answer a number."
	done
}

for work in "${WORKLOADS[@]}"; do
	color 13 "---------------------------------------------------------"
	color 13 "Result for workload $work in NPT or SPT virtualized case"
	color 13 ""

	read_num ept "Runtime of $work under EPT virtualized base page config: "
	read_num spt "Runtime of $work under SPT virtualized base page config: "

	sudo dotnet run -- "$SCRIPT_DIR/../data/" "$work" nested "$ept" "$spt"
	color 13 ""
done

bye
