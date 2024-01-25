export CC=/usr/bin/gcc-7
export CXX=/usr/bin/g++-7
export SIMULATOR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )" 
alias build_simulator='. $SIMULATOR_DIR/build.sh'
alias clean_simulator='. $SIMULATOR_DIR/clean.sh'
alias generate_trace='. $SIMULATOR_DIR/generate_trace.sh'
function run_simulator() { $SIMULATOR_DIR/build/bin64/drrun "$@"; }
