#!/usr/bin/env false

# Params:
#
# ENABLER_NAME: The name of the enabler file
# SUFFIX: The suffix for the result directory
# COMMAND: The job command to run
# VERSION: Version number of the script, must set to 2
#
# PERF_INTERVAL: Time slice length for one perf sample (perf will keep taking sample until the program exits)
# PT_MULTIPLIER: How many perf samples are required before one page table dump is taken
# START_METHOD: value={timed, file, manual} How the start of real job is signaled
#	timed: Delay a certain time period for the program to finish initialization
#	file: Wait for a signal file to be changed
#	manual: Wait for user input before start tracing (hit Enter)
# START_AFTER: Use with START_METHOD="timed", sleep certain time before the job is started
# START_FILE: Use with START_METHOD="file", start the job when the content of the file is changed

TRACER_DIR="$ARTIFACT_DIR/tracer"

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

if [ "$VERSION" != "2" ]; then
	echo "Invalid Script Version Number"
	exit
fi

sudo rm -rf "$OUTPUT_DIR" || true

sudo "$SCRIPT_DIR/prepare_system.sh"

cd "$TRACER_DIR"

echo "++++++++++++++++++++++ prehook ++++++++++++++++++++++"

pre_hook

sleep 5

echo "++++++++++++++++++++++ starting ++++++++++++++++++++++"

touch "$START_FILE"
"$TRACER_DIR/generate_trace.sh" "--output_dir=$OUTPUT_DIR" "--enabler_file=$ENABLE_FILE" $COMMAND 2>&1 & pid=$!

sleep 5

echo "++++++++++++++++++++++ posthook ++++++++++++++++++++++"

post_hook

sleep 5

cd "$SCRIPT_DIR"

echo "++++++++++++++++++++++ started (PID = $pid) ++++++++++++++++++++++"

count=99999
serial=0

if [ "$START_METHOD" = "timed" ]; then
	echo "++++++++++++++++++++++ Timed Enablement ++++++++++++++++++++++"
	sleep "$START_AFTER"
else
if [ "$START_METHOD" = "file" ]; then
	echo "++++++++++++++++++++++ Programmed File Enablement ++++++++++++++++++++++"

	monitor=$(cat "$START_FILE" 2>/dev/null)

	while true; do
		monitor2=$(cat "$START_FILE" 2>/dev/null)
		if [ "$monitor2" != "$monitor" ]; then
			break
		fi
		sleep 1
	done
else
if [ "$START_METHOD" = "manual" ]; then
	echo "++++++++++++++++++++++ Manual Enablement ++++++++++++++++++++++"
	echo "Hit Enter to start tracing"
	read __enable
fi
fi
fi

echo "++++++++++++++++++++++ enabled ++++++++++++++++++++++"

echo 1 > "$ENABLE_FILE"

while true; do

		checker=$(cat /proc/$pid/comm 2>/dev/null)
		if [ "$checker" != "generate_trace." ]; then
			break
		fi

		pid2=$(cat "$ENABLE_FILE")
		if [ "$pid2" != "1" ] && [ "$pid2" != "0" ]; then
			echo "U$pid2" | sudo tee /proc/page_tables
			sudo cat /proc/page_tables > "$OUTPUT_DIR/pt_dump_raw.$serial" || true
			# may race condition, so supress error
			sudo pmap -XX "$pid2" > "$OUTPUT_DIR/pmap_raw.$serial" || true

			if [ ! -d "/proc/$pid2" ]; then
				break
			fi
		else
			sleep "$PERF_INTERVAL"
		fi

		serial=$((serial + 1))
done

sudo python3 "$SCRIPT_DIR/../transform_pt_guest.py" "$OUTPUT_DIR"
sudo perl "$SCRIPT_DIR/../transform_pt_host.pl" "$OUTPUT_DIR/pt_dump"

sync

bye
