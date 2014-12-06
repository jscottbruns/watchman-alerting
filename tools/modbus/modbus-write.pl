#!/usr/bin/perl
use strict;

use Modbus::Client;
use FileHandle;
use Socket;
use Getopt::Long;

my $dev = 1;
my $default_on = 65280;
my $timeout = 10;
my ($host, $port);
my $DEBUG = 0;
my %addr;

my $default_on = 65280;

GetOptions (
    "timeout|t:i"	=> \$timeout,
    "host|h:s"   	=> \$host,
    "port|p:i"   	=> \$port,
    "dev:i"    		=> \$dev,
    "debug|d:i"		=> \$DEBUG,
    "addr|a=s{,}"		=> \%addr
);

die "Host address/port not specified\n" unless $host && $port;
die "No address/value arguments specified\n" unless %addr;
#for my $_i ( keys %addr )
#{
#    print "$_i => $addr{$_i}\n";
#}
#exit;
my $fd = FileHandle->new;

my $remote = inet_aton($host) || die "No such host $host";
socket($fd, PF_INET, SOCK_STREAM, getprotobyname('tcp')) || die "Can't create socket: $!";

my $paddr = sockaddr_in($port, $remote);
connect($fd, $paddr) || die "Can't connect to $host:$port - $!";

$fd->autoflush(1);

my $bus = Modbus::Client->new( $fd ) || die "Can't open Modbus client: $@ $!\n";
my $unit = $bus->device( $dev ) or die "Can't set new device: $@ $!\n";

$unit->write( %addr );

exit 0;
