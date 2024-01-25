#!/usr/bin/env false

OUTPUT_FILE="$TRACE_DIR/sim.log"
TRACE=$(find "$TRACE_DIR" -maxdepth 1 -name "drmemtrace*" -type d)

"$ARTIFACT_DIR/simulator-native/build/bin64/drrun" -t drcachesim \
                    -indir "$TRACE" \
                    -pt_dump_file "$TRACE_DIR/pt_dump" \
                    -warmup_refs     300000000                   \
                    -TLB_L1I_entries 128                          \
                    -TLB_L1I_assoc   8                           \
                    -TLB_L1D_entries 64                          \
                    -TLB_L1D_assoc   4                           \
                    -TLB_L2_entries  1536                       \
                    -TLB_L2_assoc   12                            \
                    -L1I_size  $(( 32 * 1024 ))                 \
                    -L1I_assoc 8                                 \
                    -L1D_size  $(( 32 * 1024 ))                 \
                    -L1D_assoc 8                                 \
                    -L2_size   $(( 1 * 1024 * 1024 ))           \
                    -L2_assoc  16                                \
                    -LL_size   $(( 11 * 1024 * 1024 * 16 / 8 ))   \
                    -LL_assoc  11                                \
                    -cores 20                               \
                    2>&1 | tee "$OUTPUT_FILE"

sync

sudo rm "$TRACE/drmemtrace.trace"

sync

echo "--- SIM DONE $TRACE_DIR ---"
