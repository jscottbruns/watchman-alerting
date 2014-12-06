#!perl
use Date::Format;
use Getopt::Long;
use File::Slurp;

$| = 1;

BEGIN
{
    push(@INC, "./lib");

    use constant PATH => $ENV{WATCHMAN_HOME};
    use constant TEMP_DIR => "tmp";

    use vars qw( $PATH $TEMP_DIR $DEBUG );

    if ( ! $0 || $0 ne 'Serial_Reset.pl' ) {

        open STDOUT, '>>', PATH . "\\serial_reset.log";
        open STDERR, '>>', PATH . "\\serial_reset.err"
    }
}

if ( ! $ENV{'WATCHMAN_HOME'} ) {

    &write_log("Environment variable %WATCHMAN_HOME% is not set. Cannot continue\n", 1);
    Win32::MsgBox("Please make sure that the environment variable %WATCHMAN_HOME% has been defined and is pointing to the installation directory");

    exit;
}

$PATH = PATH;
$TEMP_DIR = "$PATH\\" . TEMP_DIR;
$DEBUG = 0;

use Watchman::Serial;

my $file = "default.xml";
my $timeout = 120;
my $port = 'COM1';
my ($protocol, $lock, $current_lock);

GetOptions (
    "timeout:i" 	=>  \$timeout,
    "file:s"    	=>  \$file,
    "port:s"    	=>  \$port,
    "protocol:s"	=>	\$protocol,
    "debug"     	=>  \$DEBUG,
    "lock=i"    	=>  \$lock
);

&main::write_log("Initiating Serial Reset utility or port $port \n");

&main::write_log("Checking XML file validity on $file\n");

if ( ! -f "$TEMP_DIR\\$file" ) {

    &main::write_log("Error. File $file does not exist or cannot be accessed. Unable to proceed.\n", 1);
    exit;
}

unless ( $lock ) {

    &main::write_log("Error. Calling application failed to provide serial session lock. Unable to proceed.\n", 1);
    exit;
}

&main::write_log("Pausing for $timeout seconds before sending serial reset signal\n");

sleep $timeout;

&main::write_log("Checking serial lock against current session identifier: $lock \n");

if ( -f "$TEMP_DIR\\serial_lock.txt" ) {

	$current_lock = read_file("$TEMP_DIR\\serial_lock.txt");

	if ( $current_lock && $current_lock == $lock ) {

	    &main::write_log("Serial lock matches, sending serial reset signal\n");

		unless (
		    Watchman::Serial->new(
		        'xml_file' 		=>  $file,
		        'protocol'		=>	$protocol,
		        'port'     		=>  $port
		) ) {

			&main::write_log("Error received during serial reset attempt\n", 1);
		}

	    sleep 1;

	    &main::write_log("Sending redundant serial reset signal\n");

	    unless (
	        Watchman::Serial->new(
	            'xml_file' 		=>  $file,
	            'protocol'		=>	$protocol,
	            'port'     		=>  $port
	    ) ) {

	        &main::write_log("Error received during redundant serial reset attempt\n", 1);
	    }

	    &main::serial_lock_reset;
	    &main::write_log("Serial reset is complete\n");

	    exit(0);
	}

	&main::write_log( ( $current_lock ? "Failed to match serial session lock [$current_lock]. Serial reset aborted. \n" : "Serial session lock is not set. Serial reset no required at this time." ) );
	exit;
}

&main::write_log("Serial session lock is not set. Serial reset no required at this time.");
exit;

sub write_log
{
    my $msg = shift;
    my $err = shift;
    my ($package, $file, $line) = caller;

    $msg =~ s/\n$//;
    $msg .= " (" . $$ . ")" if substr($$, 0, 1) eq '-';
    $msg = "[" . Date::Format::time2str('%C', time) . "] [" . ( $package eq 'main' ? 'Serial_Reset' : $package ) . ":" . $line . "] " . ($err ? "[error] " : undef) . " $msg\n";

    unless ( $err ) { print STDOUT $msg }
    else { print STDOUT $msg }
}

sub serial_lock_reset
{
    open(SERIAL_LOCK, ">$TEMP_DIR\\serial_lock.txt");
    close SERIAL_LOCK;
}
