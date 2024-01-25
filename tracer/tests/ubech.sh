#/bin/bash 

# Auxilary functions section 
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

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

# MAIN SETTINGS SECTION
# Example:
SIZE=$1
if [ -z "$SIZE"]; then
  SIZE=10240
fi
OUTPUT_DIR=/disk/local/traces/ubench_$SIZE
APPLICATION="/disk/local/neuralwalkers/workloads/latency-measurements/static_mes $SIZE /disk/memphis/data/isca19/datasets/tlb_stress/${SIZE}MB.pattern 0"

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

ENABLE_FILE=/tmp/$RANDOM
echo 0 > $ENABLE_FILE 

$TRACER_DIR/generate_trace.sh --output_dir=$OUTPUT_DIR --enabler_file=$ENABLE_FILE $APPLICATION 2>&1 & pid=$!
# wait for application to warm up
sleep 20 

#once the application is warmed up 
echo 1 > $ENABLE_FILE 
wait $pid

# postprocess page table dump:
echo -n "Postprocess page table dump file to prepare it for the simulator..."
cat $OUTPUT_DIR/pt_dump_raw | $TRACER_DIR/parse_va_from_dump.py > $OUTPUT_DIR/pt_dump
# add number of lines on top of the file to simlify reading
sed -i "1i $(wc -l $OUTPUT_DIR/pt_dump | awk '{print $1}')" $OUTPUT_DIR/pt_dump
echo "Done"
