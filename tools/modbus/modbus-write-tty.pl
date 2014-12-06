#!/usr/bin/perl
use strict;

use Modbus::Client;
use FileHandle;
use Socket;
use Getopt::Long;

my $dev = 1;
my $DEBUG = 0;
my %addr;
my $port = "/dev/ttyUSB0";

my $default_on = 1;

GetOptions (
    "port|p:i"   	=> \$port,
    "dev:i"    		=> \$dev,
    "debug|d:i"		=> \$DEBUG,
    "addr|a=s{,}"		=> \%addr
);

die "Serial port not specified\n" unless $port;
die "No address/value arguments specified\n" unless %addr;
for my $_i ( keys %addr )
{
    print "$_i => $addr{$_i}\n";
}
#exit;

my $bus = new Modbus::Client $port or die "Can't open new serial port\n";
my $unit = $bus->device( $dev ) or die "Can't set new device: $@ $!\n";

$unit->write( %addr );

print "Done\n";

exit 0;
