#!/usr/bin/perl --
use strict;

use POSIX qw( strftime );
use Getopt::Long;
use File::Slurp;
use File::Spec;
use DBIx::Connector;
use Exception::Class::DBI;
use Config::Tiny;

$| = 1;

BEGIN
{
    push @INC, "/usr/local/watchman-alerting/lib";

	use constant ROOT_DIR	=> '/usr/local/watchman-alerting';
	use constant LOG_DIR	=> '/var/log/watchman-alerting';
	use constant LOG_FILE	=> 'watchman-alerting.log';
	use constant TEMP_DIR	=> '/tmp';
	use constant CONF_FILE	=> '/etc/watchman.conf';

	use vars qw( $PATH $TEMP_DIR $DEBUG $log $DBH $self );

	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

$PATH = ROOT_DIR;
$TEMP_DIR = TEMP_DIR;
$DEBUG = 0;

use Log::Dispatch;
use Log::Dispatch::File;
use Watchman::TTY_Display;

if ( $log = Log::Dispatch->new )
{
	$log->add(
		Log::Dispatch::File->new(
			name		=>	'file',
			min_level	=>	'info',
			mode		=>	'append',
			filename	=>	File::Spec->catfile( LOG_DIR, LOG_FILE ),
			callbacks	=>	sub {
				my %h = @_;
				return strftime("%Y-%m-%d %H:%M:%S", localtime()) . " " . $h{message} . "\n";
			}
		)
	);
}

&log("Opening system configuration file [ " . CONF_FILE . "]");

my $Config;

unless ( $Config = Config::Tiny->read( CONF_FILE ) )
{
	&log("Can't read configuration file: " . Config::Tiny->errstr, E_ERROR);
	die "Can't read configuration file: ", Config::Tiny->errstr;
}

$DBH = &main::init_db;
&log("Loading system settings");

$self = {};

eval {
	$self = $DBH->run( sub {
		my $arr = $_->selectall_arrayref(
			q{ SELECT * FROM Settings WHERE Category = 'serial' },
			{ Slice => {} }
		);
		my $self = {};
		foreach my $_i ( @$arr )
		{
		    $self->{ $_i->{Category} }->{ $_i->{Name} } = $_i->{Setting};
		}
		return $self;
	} )
};

if ( my $ex = $@ )
{
	&log("Database exception received when loading system settings, unable to proceed with WatchmanAlerting startup " . $ex->error, E_ERROR);
	die "Database exception received when loading system settings, unable to proceed with WatchmanAlerting startup " . $ex->error;
}


my $timeout = 120;
my $xmlfile = File::Spec->catfile( $PATH, "default.xml" );
my $port = '/dev/tty0';
my $protocol = undef;
my $ttylock = undef;
my $current_ttylock = undef;
my $xsl = undef;

GetOptions (
    "time:s"	=> \$timeout,
    "lock:s"	=> \$ttylock,
    "file:s"	=> \$xmlfile,
    "port:s"	=> \$port,
    "proto:s"	=> \$protocol,
    "xsl:s"		=> \$xsl,
    "debug"		=> \$DEBUG
);

&log("Initiating Serial Reset utility on TTY device => $port ");

&log("Checking XML file validity on file => $xmlfile");

unless ( -f $xmlfile )
{
    &log("Error locating specified XML file [ $xmlfile ] - File does not exist - Unable to continue with TTY LED reset utility", E_ERROR);
    exit 1;
}

unless ( $ttylock )
{
    &log("Error reading TTY device lock - No TTY lock identifier specified - Unable to continue with TTY LED reset utility", E_ERROR);
    exit 1;
}

&log("$timeout second wait to TTY device reset");
sleep $timeout;

&log("Comparing if TTY device lock [$ttylock] is still active");

if ( -f File::Spec->catfile( $PATH, 'tty_lock.lock' ) )
{
	$current_ttylock = read_file( File::Spec->catfile( $PATH, 'tty_lock.lock' ) );

	if ( $current_ttylock && $current_ttylock == $ttylock )
	{
	    &log("TTY device lock still in place, initiating LED display reset");

		unless (
		    Watchman::TTY_Display->new(
		        'xml_file'	=> $xmlfile,
		        'protocol'	=> $protocol,
		        'port'		=> $port,
		        'xsl_file'	=> $xsl
		) )
		{
			&log("Serial display error during LED reset", E_ERROR);
		}

	    sleep 1;

	    &log("Sending redundant serial reset signal");

	    unless (
	        Watchman::TTY_Display->new(
		        'xml_file'	=> $xmlfile,
		        'protocol'	=> $protocol,
		        'port'		=> $port,
		        'xsl_file'	=> $xsl
	    ) )
	    {
	        &log("Serial display error during LED reset", E_ERROR);
	    }

		&log("Resetting TTY device lock file");

	    if ( open(TTY_LOCK, '<', File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) )
	    {
	    	truncate TTY_LOCK, 0;
		    close TTY_LOCK;
	    }

	    &log("LED display reset complete");
	    exit(0);
	}

	&log( ( $current_ttylock ? "Failed to match TTY device lock [$current_ttylock] - LED display reset cancelled" : "TTY device lock not set - LED display reset not required" ) );

	exit 0;
}

&log("TTY device lock not set - LED display reset not required");

exit 0;

sub log
{
    my $msg = shift;
    my $level = shift;
    my ($package, $file, $line) = caller;
    $package = 'Watchman_LEDReset' if $package eq 'main';

	$level = E_WARN if ! $level;

    $msg = "[$package:$line] $msg ($$)";

    $log->$level($msg);
}

sub init_db
{
	my $dsn = "dbi:$Config->{database}->{driver}:$Config->{database}->{db_name};" .
	( $Config->{database}->{socket} ?
		"socket=$Config->{database}->{socket};" : "host=$Config->{database}->{host};port=$Config->{database}->{port};"
	);

	$log->warning("Opening database connection => $dsn");

	my $conn;
	unless (
		$conn = DBIx::Connector->new(
			$dsn,
	        $Config->{database}->{'user'},
	        $Config->{database}->{'pass'},
	        {
	        	PrintError	=> 0,
	        	RaiseError	=> 0,
	        	HandleError	=> Exception::Class::DBI->handler,
	    	    AutoCommit	=> 1,
	        }
		)
	) {

		$log->error("Database connection error: " . $DBI::errstr);
		die "Database connection error: " . $DBI::errstr;
	}

	$log->warning("Setting default DBIx mode => 'fixup' ");

	unless ( $conn->mode('fixup') )
	{
		$log->error("Error received when attempting to set default mode on line ", __LINE__, ": ", $DBI::errstr);
		die "Unable to set default SQL mode on line " . __LINE__ . ". Fatal.";
	}

	if ( $Config->{database}->{debug} )
	{
		$log->warning("Registering database handler callback debugging functions");
		$conn->{Callbacks} =
		{
	    	connected	=> sub
	    	{
	    		my ($_dbh, $_sql, $_attr) = @_;
	    	    $log->warning("[SQL DEBUG] DBI connection established");
	    	    return;
			},
	    	prepare		=> sub
	    	{
				my ($_dbh, $_sql, $_attr) = @_;
				$log->warning("[SQL DEBUG] q{$_sql}");
				return;
			}
		}
	}

	return $conn;
}