#!/usr/bin/perl
use Modbus::Client;

my $dev = $ARGV[0];
die "No device specified!\n" unless $dev;
my $addr = $ARGV[1];
die "No address specified!\n" unless $addr;
my $val = $ARGV[2];

my $bus = new Modbus::Client "/dev/ttyUSB0", POSIX::B9600 or die "Can't open Modbus client: $@ $!\n";
my $unit = $bus->device($dev) or die "Can't set new device: $@ $!\n";

$val = 0 unless $val;
print "Writing Addr [$addr=$val] on Device [$dev]\n";
my $hash = {
	$addr	=> $val
};

$unit->write( $hash );
