#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

cd "$SCRIPT_DIR/../ycsb"
mkdir -p results

./bin/ycsb load memcached -s -P "workloads/perfeval" -p "memcached.hosts=127.0.0.1" 2>&1

sleep 5

echo "++++++++++++++++++++++++++++++++ real work ++++++++++++++++++++++++++++++++++"

now=$(date)
echo "$now" > /tmp/enablement/memcached_watch

if [ "$1" = "perf" ]; then
	thp_mode="$(sudo cat /sys/kernel/mm/transparent_hugepage/enabled | perl -pe 's/^.*\[(.*)\].*$/$1/gm')"
	echo -e "\n\nsignalling readyness to /tmp/enablement/memcached_watch\n\n" >> "$SCRIPT_DIR/../../data/perf-memcached/perf-$thp_mode.log"
fi

./bin/ycsb run memcached -s -P "workloads/perfeval" -p "memcached.hosts=127.0.0.1" 2>&1

sudo killall memcached
