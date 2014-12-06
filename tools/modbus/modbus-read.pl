#!/usr/bin/perl
use strict;

use Modbus::Client;
use FileHandle;
use Socket;
use Getopt::Long;

my $dev = 1;
my $default_on = 65280;
my $timeout = 10;
my ($host, $port, $addr);
my $addr_count = 1;
my $DEBUG = 0;

GetOptions (
    "timeout|t:i"	=> \$timeout,
    "host|h:s"   	=> \$host,
    "port|p:i"   	=> \$port,
    "addr|a:i"		=> \$addr,
    "dev:i"    		=> \$dev,
    "debug|d:i"		=> \$DEBUG
);

die "Host address/port not specified\n" unless $host && $port;

my $fd = FileHandle->new;

my $remote = inet_aton($host) || die "No such host $host";
socket($fd, PF_INET, SOCK_STREAM, getprotobyname('tcp')) || die "Can't create socket: $!";

my $paddr = sockaddr_in($port, $remote);
connect($fd, $paddr) || die "Can't connect to $host:$port - $!";

$fd->autoflush(1);

my $bus = Modbus::Client->new( $fd ) || die "Can't open Modbus client: $@ $!\n";
my $unit = $bus->device( $dev ) or die "Can't set new device: $@ $!\n";

print STDERR "Reading channel [$addr] on host [$host:$port] from device [$dev]\n" if $DEBUG;

if ( $addr_count > 1 ) 
{
	
} 
else
{
	my $val = $unit->read_one( $addr );
	print STDERR "ADD:$addr VAL:$val \n" if $DEBUG;
	print STDOUT $val;
}