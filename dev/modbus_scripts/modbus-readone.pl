#!/usr/bin/perl
use Modbus::Client;
use Data::Dumper;

my $dev = $ARGV[0];
die "No device specified!\n" unless $dev;
my $addr = $ARGV[1];
die "No address specified!\n" unless $addr;

my $bus = new Modbus::Client "/dev/ttyUSB0", POSIX::B9600 or die "Can't open Modbus client: $@ $!\n";
my $unit = $bus->device($dev) or die "Can't set new device: $@ $!\n";

print "Reading Addr [$addr] on Device [$dev]\n";
my $_res  = $unit->read_one( $addr );
print "Response: $_res\n";

