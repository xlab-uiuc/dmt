#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

. "$SCRIPT_DIR/workloads.sh"

ARTIFACT_DIR="$SCRIPT_DIR/.."

cd "$ARTIFACT_DIR/simulator-native"

for work in "${WORKLOADS[@]}"; do
	TRACE_DIR="$ARTIFACT_DIR/data/trace-$work"
	. "$ARTIFACT_DIR/simulator-native/run_common.sh"
done
