#!/usr/bin/perl
use IPC::Run qw( run timeout );

if ( open(FH, "</var/log/watchman-alerting/ebxAcuyUve") )
{
	while ( <FH> )
	{
		$print .= $_;
	}
	close FH;
}

my $err;
run [ 'enscript', '-B', '-p', '-' ], '<', \$print, '|', [ 'rlpr', '--silent', '--printer=HP1320-SERVER@192.168.1.3' ], '2>', \$err, timeout( 5 );
print "ERR: $err\n" if $err;

