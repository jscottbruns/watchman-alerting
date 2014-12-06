#!/usr/bin/perl --
use IO::Socket qw(:DEFAULT :crlf);
my $addr = $ARGV[0];
my $comm = $ARGV[1];

die "Device address not specified \n" if ! $addr;

if ( ! $comm )
{
	$comm = `xsltproc xml/alphasign.xsl dev/testserial.xml`;
}

die "Command not specified \n" if ! $comm;

$/ = CRLF;

my $sock = new IO::Socket::INET(
	LocalPort=> '4000',
	PeerHost => $addr,
	PeerPort => '4000',
	Proto	 => 'udp',
	Timeout  => 3
) or die "Can't connect";

$sock->autoflush(1);

print "Sending command [$comm] to device [$addr] ...\n";
print $sock $comm, CRLF;

chomp(my $res = <$sock>);
print "Result: $res\n\n";
