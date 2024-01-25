#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="redis"

sudo service redis stop || true
sudo killall redis-server || true
sudo killall bench_redis_st || true
sudo rm ./dump.rdb || true

. "$SCRIPT_DIR/config-$1.sh"

function pre_hook { :; }
function post_hook { :; }

COMMAND="$ARTIFACT_DIR/workloads/bin/bench_redis_st"

START_FILE="/tmp/enablement/redis_watch"

. "$SCRIPT_DIR/common-$1.sh"
