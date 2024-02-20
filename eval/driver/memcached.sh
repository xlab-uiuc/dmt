#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/../../scripts/common.sh"

SUFFIX="memcached"

sudo service memcached stop || true
sudo killall memcached || true

. "$SCRIPT_DIR/config-$1.sh"

param="$1"

function pre_hook { :; }

function post_hook {
	"$SCRIPT_DIR/memcached-client.sh" "$param" &
}

COMMAND="/usr/bin/memcached -u root -m 131000 -p 11211 -l 127.0.0.1"

START_FILE="/tmp/enablement/memcached_watch"

. "$SCRIPT_DIR/common-$1.sh"
