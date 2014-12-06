#!/usr/bin/perl
use strict;

use Modbus::Client;
use FileHandle;
use Socket;
use Getopt::Long;

my $dev = 1;
my $default_on = 1;
my $port = '/dev/ttyUSB0';
my $addr;
my $addr_count = 1;
my $DEBUG = 0;

GetOptions (
    "port|p:i"   	=> \$port,
    "addr|a:i"		=> \$addr,
    "dev:i"    		=> \$dev,
    "debug|d:i"		=> \$DEBUG
);

die "Host address/port not specified\n" unless $port;

my $bus = new Modbus::Client $port or die "Can't open new serial port\n";
my $unit = $bus->device( $dev ) or die "Can't set new device: $@ $!\n";

print STDERR "Reading channel [$addr] on port [$port] from device [$dev]\n" if $DEBUG;

if ( $addr_count > 1 ) 
{
	print "Error reading multiple address values - Retry with single address\n";
    exit;
} 
else
{
	my $val = $unit->read_one( $addr );
	print STDERR "ADD:$addr VAL:$val \n" if $DEBUG;
	print STDOUT $val;
}
