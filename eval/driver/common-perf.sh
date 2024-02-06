#!/usr/bin/env false

PERF_PATH="$ARTIFACT_DIR/linux/tools/perf/perf"

PERF_DTLB_LOAD="dtlb_load_misses.walk_completed,dtlb_load_misses.walk_duration,"
PERF_DTLB_STORE="dtlb_store_misses.walk_completed,dtlb_store_misses.walk_duration,"
PERF_ITLB="itlb_misses.walk_completed,itlb_misses.walk_duration,"
PERF_OTHER= "ept.walk_cycles," # For some CPUs, this may not present

PERF_COMMON="cycles:ukhHG,task-clock:ukhHG,cpu-clock:ukhHG"
PERF_ITEMS="$PERF_DTLB_LOAD$PERF_DTLB_STORE$PERF_ITLB$PERF_OTHER$PERF_COMMON"

cd "$SCRIPT_DIR"

kill_descendant_processes() {
	local pid="$1"
	local and_self="${2:-false}"
	if children="$(pgrep -P "$pid")"; then
		for child in $children; do
			kill_descendant_processes "$child" true
		done
	fi
	if [[ "$and_self" == true ]]; then
		echo "Killing $pid"
		kill -INT "$pid"
	fi
}

sigint()
{
	echo "signal INT received, script ending"
	kill_descendant_processes $$
	exit 0
}
trap sigint SIGINT

post_hook_launcher()
{
	sleep 5

	echo "++++++++++++++++++++++ posthook ++++++++++++++++++++++"

	post_hook
}

echo "++++++++++++++++++++++ prehook ++++++++++++++++++++++"

pre_hook

sleep 5

echo "++++++++++++++++++++++ starting ++++++++++++++++++++++"

post_hook_launcher &

thp_mode="$(sudo cat /sys/kernel/mm/transparent_hugepage/enabled | perl -pe 's/^.*\[(.*)\].*$/$1/gm')"

sudo "$PERF_PATH" stat record -e "$PERF_ITEMS" -o "$OUTPUT_DIR/perf.data" -I 5000 $COMMAND >> "$OUTPUT_DIR/perf-$thp_mode.log" 2>&1

sudo python3 "$SCRIPT_DIR/../page_walk_counter.py" "$OUTPUT_DIR/perf-$thp_mode.log" > "$OUTPUT_DIR/perf-$thp_mode.short"

sync

bye
