#!/usr/bin/perl --
use IO::Socket qw(:DEFAULT :crlf);
#my $addr = $ARGV[0];
my $port = $ARGV[0];
my $comm = $ARGV[1];

die "Command not specified \n" if ! $comm;
die "Port not specified \n" if ! $port;

$/ = CRLF;
$| = 1;

my $sock = new IO::Socket::INET(
	PeerAddr => inet_ntoa(INADDR_BROADCAST),
	PeerPort => $port,
    LocalPort   => $port,
	Proto	 => udp,
    Broadcast => 1,
) or die "Can't connect";

print "Sending command [$comm] to device [$addr:$port] ...\n";

$sock->autoflush(1);    
print $sock $comm;
sleep 1;
$sock->close;       
