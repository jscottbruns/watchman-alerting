#!/usr/bin/perl
use IO::Socket qw(:DEFAULT :crlf);

my $addr = '';
my $port = '';

my $ledcmd = "xsltproc xsl/alphasign.xsl test.xml";
my $ledtxt = `$ledcmd`;

my $sock;
$/ = CRLF;

$sock = new IO::Socket::INET(
	PeerAddr => inet_ntoa(INADDR_BROADCAST),
	PeerPort => '5052',
	LocalPort => '5052',
	Proto    => 'udp',
	Broadcast 	=> 1
) or die "Socket error: $@ $!\n";

if ( $sock )
{
    $sock->autoflush(1);
    print "Sending command\n";
    print $sock $ledtxt;
    sleep 1;
    $sock->close;

    print "All done\n";
}
else
{
print "N";
}
print "Done";
