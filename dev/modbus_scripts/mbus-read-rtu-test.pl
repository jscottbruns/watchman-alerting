#!/usr/bin/perl

use Modbus::Client;

my $dev = 1;
my $default_on = 65280;
my $timeout = 10;
my $addr = '10001';
my $addr_count = 1;
my $DEBUG = 0;

my $bus = new Modbus::Client "COM2", POSIX::B9600;
my $unit = $bus->device( $dev ) or die "Can't set new device: $@ $!\n";

print STDERR "Reading channel [$addr] on host [$host:$port] from device [$dev]\n" if $DEBUG;

my $val = $unit->read_one( $addr );
print STDERR "ADD:$addr VAL:$val \n";
print STDOUT $val;
