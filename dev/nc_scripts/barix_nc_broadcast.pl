#!/usr/bin/perl --
use IO::Socket qw(:DEFAULT :crlf);
#my $addr = $ARGV[0];
my $port = $ARGV[0];
my $comm = $ARGV[1];

die "Command not specified \n" if ! $comm;
die "Port not specified \n" if ! $port;

$/ = CRLF;

my $sock = new IO::Socket::INET(
	PeerAddr => inet_ntoa(INADDR_BROADCAST),
	PeerPort => $port,
	Proto	 => udp,
    LocalAddr => '10.100.1.50',
    Broadcast => 1,
    Timeout => 3
) or die "Can't connect";

$sock->autoflush(1);

print "Sending command [$comm] to device [$addr:$port] ...\n";
print $sock $comm, CRLF;

chomp(my $res = <$sock>);
print "Result: $res\n\n";
