#!/usr/bin/perl --
use IO::Socket qw(:DEFAULT :crlf);
my $addr = $ARGV[0];
my $port = $ARGV[1];
my $comm = $ARGV[2];

die "Device address not specified \n" if ! $addr;
die "Command not specified \n" if ! $comm;
die "Port not specified \n" if ! $port;

$/ = CRLF;

print "Opening Socket to Device [$addr:$port] ...\n";

my $sock = new IO::Socket::INET(
	PeerHost => $addr,
	PeerPort => $port,
	Proto	 	=> 'udp'
) or die "Can't connect";

$sock->autoflush(1);

print "Sending command [$comm] to device [$addr:$port] ...\n";
print $sock $comm, CRLF;

sleep(1);

$sock->close ();
print "Socket Closed\n";
