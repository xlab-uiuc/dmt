#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="debug"

sudo killall bench_gups_st || true

. "$SCRIPT_DIR/config-$1.sh"

function pre_hook { :; }
function post_hook { :; }

COMMAND="$ARTIFACT_DIR/workloads/bin/bench_gups_st 16"

START_FILE="/tmp/enablement/gups_watch"

. "$SCRIPT_DIR/common-$1.sh"
