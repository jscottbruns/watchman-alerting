#!perl
use strict;
use File::Copy;
use Date::Format;
use DBI;
use Win32::Process;
use Win32::Process::Info;

BEGIN {

    use constant PATH => $ENV{WATCHMAN_HOME};

    use vars qw( $PATH );

    # Create an archive of the log file
    copy( PATH . "\\watchman_helper.log", PATH . "\\watchman_helper.log.1" ) if -f PATH . "\\watchman_monitor.log";

    if ( ! $0 || $0 ne 'Watchman_Helper.pl' ) {
        open (STDOUT, '>'. PATH . "\\watchman_helper.log") or die "Couldn't dup stdout to filelog: $!\n";
        open (STDERR, '>'. PATH . "\\watchman_helper.log")or die "Couldn't dup stderr to filelog: $!\n";
    }
};

my $PATH = $ENV{'WATCHMAN_HOME'};
my $db_file = "$PATH\\watchman.s3db";
my $exec = 'Watchman.exe';

my $settings = {};

&main::write_log("Initiating Watchman_Helper to monitor memory usage\n");

if ( ! -f $db_file ) {
    &main::write_log("Unable to stat database file: $db_file\n", 1);
    exit;
}

my $dbh = DBI->connect(
    "dbi:SQLite:dbname=$db_file",
    "",
    "",
    {
       ShowErrorStatement => 1,
       PrintError => 1
    }
);
unless ( $dbh ) {

    &main::write_log("Error connecting to database: " . $DBI::errStr . "\n", 1);
    exit;
}

my %row;
my $sth = $dbh->prepare("SELECT name, setting
                         FROM settings
                         WHERE name IN ('mem_ceiling', 'mem_interval') AND category = 'system'") or die "DBH Query Error: " . $dbh->errstr;
if ( $sth->execute ) {

    $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
    while ( $sth->fetch ) {
    	$settings->{ $row{'name'} } = $row{'setting'};
    }
}

$sth->finish();
undef $sth;

$dbh->disconnect();

if ( ! $settings->{'mem_interval'} ) {
	&main::write_log("Memory interval not set, defaulting to 1200 seconds\n");
    $settings->{'mem_interval'} = 1200;
}

&main::write_log("Setting memory ceiling at $settings->{mem_ceiling} K\n") if $settings->{'mem_ceiling'};
&main::write_log("Setting monitor interval to $settings->{mem_interval} seconds\n");

unless ( $settings->{'mem_ceiling'} ) {
    &main::write_log("Memory ceiling has not been set. Unable to continue with Watchman Helper\n", 1);
    exit;
}

my ($p_name, $p_id, $p_mem);

&main::write_log("Sleeping for $settings->{mem_interval} seconds before beginning\n");
sleep $settings->{'mem_interval'};

&main::write_log("Fetching Windows tasklist for process $exec\n");

my ($running, $ProcessObj);

while ( 1 ) {

	my $tasklist = `tasklist`;
	$running = 0;

	while ( $tasklist =~ /^([A-Za-z0-9].*\.exe)\s{2,}([0-9]*)\s(.*)\s{2,}([0-9]*)\s{2,}([0-9,]*)/gm ) {
		$p_name = $1;
		$p_id = $2;
		$p_mem = $5;
		$p_mem =~ s/[^-.0-9]//g;

	    if ( $p_name =~ /$exec/i ) {

            $running = 1;
	    	&main::write_log("Identified matching process $p_name ($p_id) using $p_mem K of memory\n");

	    	if ( $p_mem >= $settings->{'mem_ceiling'} ) {

		        &main::write_log("Memory usage exceeds memory ceiling. Initiating process reboot\n");
		        my $obj;

	            &main::write_log("Inheriting process object\n");
		        if ( Win32::Process::Open($obj, $p_id, 1) ) {

		        	&main::write_log("Sending KILL signal to process\n");
			        if ( $obj->Kill(0) ) {

				        &main::write_log("Process $p_name ($p_id) has successfully been stopped\n");
				        sleep 5;

	                    &main::write_log("Restarting process '$PATH\\$exec'\n");
				        unless ( Win32::Process::Create($ProcessObj,
				                                        "$PATH\\$exec",
				                                        '',
				                                        0,
				                                        DETACHED_PROCESS,
				                                        "$PATH")
				        ) {

				            &main::write_log("Fatal error when attempting to restart $exec. Unable to continue.\n", 1);
				        }

	                    &main::write_log("Process started successfully\n");
				        undef $ProcessObj;

			        } else {

			        	&main::write_log("KILL signal failed to stop process. Aborting process reboot\n", 1);
			        }

			        undef $obj;

		        } else {

		        	&main::write_log("Unable to inherit process object. Aborting process restart\n", 1);
		        }
	    	}

	    	last;
	    }
	}

    if ( ! $running ) {

    	&main::write_log("$exec is not running and was not found in tasklist. Attempting to restart process\n", 1);
	    unless ( Win32::Process::Create($ProcessObj,
	                                    "$PATH\\$exec",
	                                    '',
	                                    0,
	                                    DETACHED_PROCESS,
	                                    "$PATH")
	    ) {

	        &main::write_log("Fatal error when attempting to restart $exec. Unable to continue.\n", 1);
	    }

	    &main::write_log("Process started successfully\n");
	    undef $ProcessObj;
    }

    &main::write_log("Sleeping for $settings->{mem_interval} seconds\n");
	sleep $settings->{'mem_interval'};
}

sub write_log
{
    my $msg = shift;
    my $err = shift;
    my ($package, $file, $line) = caller;

    $msg =~ s/\n$//;
    $msg .= " (" . $$ . ")" if substr($$, 0, 1) eq '-';
    $msg = "[" . Date::Format::time2str('%C', time) . "] [Watchman_Helper:" . $line . "] " . ($err ? "[error] " : undef) . " $msg\n";

    unless ( $err ) { print STDOUT $msg }
    else { print STDOUT $msg }
}