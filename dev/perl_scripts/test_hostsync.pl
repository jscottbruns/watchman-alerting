#!perl
use strict;
use POSIX ":sys_wait_h";
use File::Copy;
use DBI;

$| = 1;

BEGIN {

    push(@INC, "./lib");

    use constant VERSION => "1.0.0";
    use constant PATH => $ENV{WATCHMAN_HOME};
    use constant TEMP_DIR => "tmp";

    use vars qw( $PATH $TEMP_DIR $VERSION $DEBUG $LICENSE $DB_FILE $HTTP_PORT );
}

$PATH = PATH;
$TEMP_DIR = "$PATH\\" . TEMP_DIR;
$VERSION = VERSION;
$DB_FILE = 'watchman.s3db';
$LICENSE = 'MD160026';


use Watchman::AlertQueue;
use Date::Format;

my @inc;

push @inc, {
	'cno'   =>  'P12345',
	'ope'   =>  '12:25:12',
	'clo'   =>  '',
	'typ'   =>  'APTT',
	'box'   =>  '2815',
	'inc'   =>  '',
	'loc'   =>  '7611 ANNAPOLIS RD (76TH AVE & 77TH AVE)'
};

push @inc, {
	'cno'   =>  'P12346',
	'ope'   =>  '12:26:12',
	'clo'   =>  '',
	'typ'   =>  'ALS',
	'box'   =>  '3312',
	'inc'   =>  '',
	'loc'   =>  '7714 WEST FOREST RD (KENT TOWN PL & LANDOVER RD)',
};

Watchman::HostSync->new(
	'hostsync_uri'         =>  'http://www.soaplite.com/Watchman/SOAP',
	'hostsync_proxy'       =>  'http://soap.firehousewatchman.com',
	'hostsync_timeout'     =>  10,
	'hostsync_username'    =>  'watchman',
	'hostsync_password'    =>  '123123',
	'inc_array'            =>  \@inc
);

print "Done\n";
exit;


sub write_log
{
	my $msg = shift;

	print $msg;
}
