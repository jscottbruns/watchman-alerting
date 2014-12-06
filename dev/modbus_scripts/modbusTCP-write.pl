#!/usr/bin/perl
use strict;

use Modbus::Client;
use FileHandle;
use Socket;

my $host = $ARGV[0];
my $port = $ARGV[1];
my $addr = $ARGV[2];
my $val = $ARGV[3];
my $dev = $ARGV[4] || 1;

my $default_on = 65280;

die "Host address/port not specified\n" unless $host && $port;
die "Modbus device address/value not specified\n" unless $addr && $val >= 0;

my $fd = FileHandle->new;

my $remote = inet_aton($host) || die "No such host $host";
socket($fd, PF_INET, SOCK_STREAM, getprotobyname('tcp')) || die "Can't create socket: $!";

my $paddr = sockaddr_in($port, $remote);
connect($fd, $paddr) || die "Can't connect to $host:$port - $!";

$fd->autoflush(1);

my $bus = Modbus::Client->new( $fd ) || die "Can't open Modbus client: $@ $!\n";
my $unit = $bus->device( $dev ) or die "Can't set new device: $@ $!\n";

print "Writing channel [$addr=$val] on host [$host:$port] to device [$dev]\n";

my $hash = 
{
	$addr	=> $val
};

$unit->write( {
	$addr => $hash 
} ) or die "Can't write to modbus device: $@ $! \n";