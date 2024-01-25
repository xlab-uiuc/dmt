#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="graph500"

sudo killall bench_graph500_st || true

. "$SCRIPT_DIR/config-$1.sh"

function pre_hook { :; }
function post_hook { :; }

COMMAND="$ARTIFACT_DIR/workloads/bin/bench_graph500_st -- -s 27 -e 32 -V"

START_FILE="/tmp/enablement/graph500_watch"

. "$SCRIPT_DIR/common-$1.sh"
