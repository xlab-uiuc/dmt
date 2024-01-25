#!/usr/bin/env false

cd "$SCRIPT_DIR"

ARTIFACT_DIR="$(realpath -e "$SCRIPT_DIR/../..")"

OUTPUT_DIR="$ARTIFACT_DIR/data/$TAG-$SUFFIX/"

if [ $CREATE_OUTPUT -eq 1 ]; then
	mkdir -p "$OUTPUT_DIR"
fi

mkdir -p "/tmp/enablement"
ENABLE_FILE="/tmp/enablement/$SUFFIX"
echo 0 > "$ENABLE_FILE"
