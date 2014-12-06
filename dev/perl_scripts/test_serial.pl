#!perl
use strict;
use POSIX ":sys_wait_h";
use File::Copy;

$| = 1;

BEGIN {

    push(@INC, "../lib");

    use constant VERSION => "1.0.0";
    use constant PATH => $ENV{WATCHMAN_HOME};
    use constant TEMP_DIR => "tmp";

    use vars qw( $PATH $TEMP_DIR $VERSION $DEBUG $LICENSE $DB_FILE );
};

if ( ! $ENV{'WATCHMAN_HOME'} ) {

    Win32::MsgBox("Please make sure that the environment variable %WATCHMAN_HOME% has been defined and is pointing to the installation directory");
    exit;
}

$DEBUG = 1;
$PATH = PATH;
$TEMP_DIR = "$PATH\\" . TEMP_DIR;
$VERSION = VERSION;
$DB_FILE = 'watchman.s3db';

use File::Basename;
use Watchman::Serial;

use Time::HiRes;
use Date::Format;
use HTML::Entities;

my $xmlfile = $ARGV[0];
my $protocol = $ARGV[1];

if ( ! $xmlfile ) {

	print "No XML file specified for signboard display. Please specify valid XML file. XML file must exist in temp directory [ $TEMP_DIR ]\n";
	exit;
}

if ( ! -f "$TEMP_DIR\\$xmlfile" ) {

	print "Cannot stat file $xmlfile\n";
	exit;
}

unless (
	Watchman::Serial->new(
		'xml_file'       	=>  $xmlfile,
		'port'          	=>  'COM1',
		'protocol'	=>	$protocol,
	)
) {

	print "Errors during serial write\n";
	exit;
}

sub write_log
{
    my $msg = shift;
    my $err = shift;
    my ($package, $file, $line) = caller;

    $msg =~ s/\n$//;
    $msg .= " (" . $$ . ")" if substr($$, 0, 1) eq '-';
    $msg = "[" . Date::Format::time2str('%C', time) . "] [" . $package . ":" . $line . "] " . ($err ? "[error] " : undef) . " $msg\n";

    print STDOUT $msg;
}

sub trim
{
    my $string = shift;
    if ( $string ) {
        $string =~ s/^\s+//;
        $string =~ s/\s+$//;
        return $string;
    }
}