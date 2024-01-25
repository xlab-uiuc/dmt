#!/bin/bash 

SOURCE="${BASH_SOURCE[0]:-$0}";
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
        DIR="$( cd -P "$( dirname -- "$SOURCE"; )" &> /dev/null && pwd 2> /dev/null; )";
        SOURCE="$( readlink -- "$SOURCE"; )";
        [[ $SOURCE != /* ]] && SOURCE="${DIR}/${SOURCE}"; # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR="$( cd -P "$( dirname -- "$SOURCE"; )" &> /dev/null && pwd 2> /dev/null; )";

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

APPLICATION=""
READ_THE_REST=0
for i in "$@"
do
  if [ "$READ_THE_REST" -eq "1" ]; then
    APPLICATION="$APPLICATION $i"
    continue
  else
    case $i in
        -f=*|--enabler_file=*)
        ENABLE_FILE="${i#*=}"
        shift # past argument=value
        continue
        ;;
        -o=*|--output_dir=*)
        OUTPUT_DIR="${i#*=}"
        shift # past argument=value
        continue
        ;;
        *)
        READ_THE_REST=1
        APPLICATION="$i"
        ;;    
    esac
  fi
done

if [ -d "$OUTPUT_DIR" ]; then
  echo "Error: Seems that output directory $OUTPUT_DIR already exists. Please rename/remove it."
  exit 1
fi

echo "APPLICATION             = ${APPLICATION}"
echo "OUTPUT_DIR              = ${OUTPUT_DIR}"
echo "ENABLE_FILE             = ${ENABLE_FILE}"

echo "Start tracer..."
mkdir -p $OUTPUT_DIR
# launch tracer 
"$SCRIPT_DIR/build/bin64/drrun" -t drcachesim                  \
                              -offline                       \
                              -outdir "$OUTPUT_DIR"            \
                              -verbose 1                     \
                              -enabler_filename "$ENABLE_FILE" \
                              -trace_after_instrs 1          \
                                                             \
                              -exit_after_tracing 2000000000      \
                              -- $APPLICATION                \
                              & pid=$!
                              #-exit_after_tracing 100000000      \

## enable page table dump
#echo $pid > /proc/page_tables
#echo "Page table dump module was attached to PID=$pid"

wait
