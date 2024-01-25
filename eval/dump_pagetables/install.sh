#!/usr/bin/env bash

sudo rmmod dump_pagetables
make
sudo insmod dump_pagetables.ko
