#!/usr/bin/perl
use Modbus::Client;
use Data::Dumper;
use FileHandle;
use Socket;

my $fd = FileHandle->new;
my $host = '10.100.1.7';
my $port = '502';

my $remote = inet_aton($host) || die "No such host $host";
socket($fd, PF_INET, SOCK_STREAM, getprotobyname('tcp')) || die "Can't create socket: $!";

my $paddr = sockaddr_in($port, $remote);
connect($fd, $paddr) || die "Can't connect to $host:$port - $!";
$fd->autoflush(1);

my $bus = Modbus::Client->new($fd) || die "Can't open Modbus client: $@ $!\n";
my $unit = $bus->device(1) or die "Can't set new device: $@ $!\n";

my $_hash  = $unit->read(10001..10005);
print "Response: :\n";

for my $_i ( sort keys %{ $_hash } )
{
        print "$_i => $_hash->{ $_i } \n";
}


#print "Reading Addr [$addr] on Device [$dev]\n";
#my $_res  = $unit->read_one( 401);
#print "Response: $_res\n";

