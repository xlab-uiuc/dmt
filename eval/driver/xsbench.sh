#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="-xsbench"

sudo killall bench_xsbench_mt || true

. "$SCRIPT_DIR/config-$1.sh"

function pre_hook { :; }
function post_hook { :; }

COMMAND="$ARTIFACT_DIR/workloads/bin/bench_xsbench_mt2 -- -t 1 -g 170000 -p 4000000"

START_FILE="/tmp/enablement/xsbench_watch"

. "$SCRIPT_DIR/common-$1.sh"
