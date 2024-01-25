#!/usr/bin/env false

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

taskset -c 1 $COMMAND

sync

bye
