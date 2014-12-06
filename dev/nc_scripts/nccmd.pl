#!/usr/bin/perl --
use IO::Socket::INET;
my $addr = $ARGV[0];
my $port = $ARGV[1];
my $comm = $ARGV[2];

die "Device address not specified \n" if ! $addr;
die "Command not specified \n" if ! $comm;
die "Port not specified \n" if ! $port;

print "Opening Socket to Device [$addr:$port] ...\n";

$/ = CRLF;

my $sock = new IO::Socket::INET(
    PeerAddr => $addr,
    PeerPort => $port,
    Proto    => 'tcp',
) or die "Can't connect";

$sock->autoflush(1);

print "Sending command [$comm] to device [$addr:$port] ...\n";
print $sock $comm, CRLF;

sleep(1);
chomp(my $res = <$sock>);
print "Result: $res\n\n";
$sock->close ();
print "Socket Closed\n";
