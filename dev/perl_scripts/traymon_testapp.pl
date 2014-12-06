#!/usr/bin/perl
#use warnings;
use strict;

open(FH, ">..\\tmp\\traymon.out") or die "Can't open log file";

while ( 1 ) {
	print FH "Sleeping...\n";
	sleep 5;
}

close FH;