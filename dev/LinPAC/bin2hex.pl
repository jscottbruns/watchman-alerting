#!/usr/bin/perl
# http://www.binaryhexconverter.com/binary-to-hex-converter
use warnings;
use strict;

my $bin = $ARGV[0] || die "No input";
my $int = unpack("N", pack("B32", substr("0" x 32 . $bin, -32)));
my $hex = sprintf("%x", $int );
print "Bin: [$bin] Hex: [" . uc($hex) . "]\n";
