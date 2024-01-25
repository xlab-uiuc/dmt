#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="canneal"

sudo killall bench_canneal_st || true

. "$SCRIPT_DIR/config-$1.sh"

function pre_hook { :; }
function post_hook { :; }

COMMAND="$ARTIFACT_DIR/workloads/bin/bench_canneal_st 1 150000 2000 $ARTIFACT_DIR/datasets/canneal 10000"

START_FILE="/tmp/enablement/canneal_watch"

. "$SCRIPT_DIR/common-$1.sh"

