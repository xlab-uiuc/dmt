#!/usr/bin/env perl

use warnings;
use strict;

my $filename = shift;
open (FH, "+< $filename") || die "Can't open $filename: $!";

my $count = `wc -l "$filename"`;
my $line = <FH>;
my $len = length($line) - 1; # -1 to skip \n

$count =~ s/^(\d+).*/$1/;
$line = sprintf("\%0${len}d\n", $count - 1); # -1 to skip first line

seek FH, 0, 0;
printf FH $line;
close FH;
