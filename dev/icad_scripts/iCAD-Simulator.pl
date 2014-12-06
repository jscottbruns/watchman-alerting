#!/usr/bin/perl
use strict;
use IO::Socket qw(:DEFAULT :crlf);
use File::Spec;

BEGIN
{
	use constant DAEMON		=> 'watchman-alerting';
	use constant ROOT_DIR	=> '/usr/local/watchman-alerting';
	use constant LOG_DIR	=> '/var/log/watchman-alerting';
	use constant LOG_FILE	=> 'watchman-alerting.log';
	use constant TEMP_DIR	=> '/tmp';
	use constant CONF_FILE	=> '/etc/watchman.conf';
}

my $file = $ARGV[0] || File::Spec->catfile( ROOT_DIR, 'xslt/xsd/iCAD-SampleIncident.xml' );

unless ( -f $file )
{
	print "Can't stat file $file\n";
	exit;
}

my $sock;

INET_START:
eval {
	$sock = IO::Socket::INET->new(
		PeerHost	=> 'localhost',
		PeerPort	=> '8888',
		Proto		=> 'tcp',
		Type		=> SOCK_STREAM,
		Timeout		=> 10
	)
};
if ( my $ex = $@ )
{
	print "Error: $ex\n";
}

$sock->autoflush(1);

my $xml = '';
if ( open(FH, '<', $file ) )
{
	while ( <FH> )
	{
		$xml .= $_;
	}
	close FH;
}
print "Sending incident dispatch stream...\n";

print $sock $xml,CRLF,CRLF;

my $confirm;
print "Waiting for confirmation...\n";
{
	local $/ = CRLF;
	chomp( my $confirm = <$sock> );
	print "Confirm: $confirm\n";
}

print "[r] Resend iCAD Dispatch [q] Quit ";
chomp( my $u = <> );

print "Closing socket\n";
$sock->close;

goto INET_START if lc( $u ) eq 'r';

print "Done\n";
exit;

sub userinput
{
    print "Press enter to continue...";
    chomp( my $u = <> );

    die if $u eq 'q';
    return;
}
