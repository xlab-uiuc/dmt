#!/usr/bin/env perl

use warnings;
use strict;
use bigint qw/hex/;

my $maxStart = 0;
my $maxSz = 0;

while(<>) {
	next unless (/^([0-9a-fA-F]+)\s+(\d+)K\s.*\[ anon \]$/gm);

	my $start = $1;
	my $size = $2;
	
	if ($size > $maxSz) {
		$maxSz = $size;
		$maxStart = $start;
	}
}

my $end = hex($maxStart) + $maxSz * 1024;
my $hex = sprintf("%016x", $end);

print("$maxStart $hex");
