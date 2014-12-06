#!perl
use POSIX ":sys_wait_h";
use File::Copy;
use strict;

$| = 1;

BEGIN
{
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
$DEBUG = 1;

use File::Basename;
use Win32::Process;
use Time::HiRes;
use Date::Format;
use HTML::Entities;

use HTTP::Request;
use LWP::UserAgent;
use DBI;

use Watchman::HTTP;

print "Starting HTTPD daemon...\n";
unless ( Watchman::HTTP->new ) {

    print "Error starting Watchman local server\n";
    exit;
}

sub write_log
{
    my $msg = shift;

    print $msg;
}