#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

. "$SCRIPT_DIR/workloads.sh"

ARTIFACT_DIR="$SCRIPT_DIR/.."

if [ "$#" -ne "1" ]; then
	color 9 "$0 [path_to_pt_dump.host]"
	false
fi

hostfile="$(realpath -e "$1")"

cd "$ARTIFACT_DIR/simulator-native"

for work in "${WORKLOADS[@]}"; do
	TRACE_DIR="$ARTIFACT_DIR/data/trace-$work"
	ln -sf "$hostfile" "$TRACE_DIR/pt_dump.host"
	. "$ARTIFACT_DIR/simulator-virt/run_common.sh"
done
