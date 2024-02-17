#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="btree"

sudo killall bench_btree_st || true

. "$SCRIPT_DIR/config-$1.sh"

function pre_hook { :; }
function post_hook { :; }

COMMAND="$ARTIFACT_DIR/workloads/bin/bench_btree_st"

START_FILE="/tmp/enablement/btree_watch"

. "$SCRIPT_DIR/common-$1.sh"
