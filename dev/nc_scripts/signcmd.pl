#!/usr/bin/perl --
use IO::Socket qw(:DEFAULT :crlf);
my $xml = $ARGV[0];
my $addr = '10.10.0.205';
$| = 1;
die "No XML\n" if ! $xml;

my $text = `xsltproc /usr/local/watchman-alerting/xsl/alphasign.xsl $xml`;

$/ = CRLF;

my $sock = new IO::Socket::INET(
    PeerHost => $addr,
    PeerPort => '4000',
    Proto    => 'tcp',
) or die "Can't connect";

$sock->autoflush(1);

print "Sending text [$text] to device [$addr] ...\n";

print $sock $text;
sleep 1;
$sock->close;

