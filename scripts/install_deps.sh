#!/usr/bin/env bash

set -euo pipefail

# file locator
SCRIPT_DIR=$(dirname "$(realpath -e "${BASH_SOURCE[0]:-$0}")")
. "$SCRIPT_DIR/common.sh"

# Common
sudo apt-get update
sudo apt-get install -y apt-transport-https
sudo apt-get update
sudo apt-get install -y bash curl git man perl perl-doc sudo wget screen vim nano software-properties-common zip unzip tar rsync
sudo apt-get install -y python3 python3-dev python3-pip python3-venv python-is-python3

# Kernel
sudo apt-get install -y build-essential linux-tools-common linux-tools-generic liblz4-tool dwarves binutils elfutils gdb flex bison libncurses-dev libssl-dev libelf-dev
sudo apt-get install -y cmake gcc g++ make libiberty-dev autoconf zstd libboost-all-dev arch-install-scripts
sudo apt-get install -y libdw-dev systemtap-sdt-dev libunwind-dev libslang2-dev libperl-dev liblzma-dev libzstd-dev libcap-dev libnuma-dev libbabeltrace-ctf-dev libbfd-dev
sudo apt-get install -y clang clang-format clang-tools llvm

# Dotnet
sudo wget https://dot.net/v1/dotnet-install.sh -O /opt/dotnet-install.sh
sudo chmod -c +x /opt/dotnet-install.sh
sudo /opt/dotnet-install.sh --version latest --install-dir /opt/dotnet/
if sudo test -e "/opt/dotnet/dotnet" && sudo test ! -e "/usr/local/bin/dotnet"; then
	sudo ln -s /opt/dotnet/dotnet /usr/local/bin/dotnet
fi

# Workloads
sudo apt-get install -y libreadline-dev

# Check we have everything we want
python3 --version
gcc --version
cmake --version
dotnet --list-sdks

bye
