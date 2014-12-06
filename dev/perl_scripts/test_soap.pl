#!/usr/bin/perl --
package main;
use strict;

$| = 1;

BEGIN
{
	push @INC, "/usr/local/watchman-alerting/lib";

	use constant DAEMON		=> 'watchman-alerting';
	use constant ROOT_DIR	=> '/usr/local/watchman-alerting';
	use constant LOG_DIR	=> '/var/log/watchman-alerting';
	use constant LOG_FILE	=> 'watchman-alerting.log';
	use constant TEMP_DIR	=> '/tmp';
	use constant CONF_FILE	=> '/etc/watchman.conf';

	use vars qw( $PATH $TEMP_DIR $DEBUG $PID_FILE $CONF_FILE $log $DBH $self $LOG_DIR $LICENSE $CONTINUE $CONTINUEPOLL %PIDS );

	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warn';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}


$PATH = ROOT_DIR;
$TEMP_DIR = TEMP_DIR;
$LOG_DIR = LOG_DIR;
$CONF_FILE = CONF_FILE;

$LICENSE = 'MD160026';

use Text::Wrap;
use Text::Template;
use LWP::UserAgent;
use HTTP::Headers;
use HTTP::Message;
use HTTP::Cookies;
use SOAP::Lite;
use Time::HiRes;
use Date::Format;
use HTML::Entities;
use Storable;
use Config::Tiny;
use DBIx::Connector;
use Exception::Class::DBI;

my $Config;
&log("Reading system configuration file [ " . CONF_FILE . "]");

unless ( $Config = Config::Tiny->read( CONF_FILE ) )
{
	&log("Can't read configuration file: " . Config::Tiny->errstr, E_ERROR);
	die "Can't read configuration file: ", Config::Tiny->errstr;
}

$DEBUG = $Config->{'system'}->{'debug'};
&log("Setting DEBUG flag => [$DEBUG]");

$LICENSE = $Config->{'system'}->{'license'};
&log("Setting license => [$LICENSE]");

$DBH = &main::init_db;

&log("Loading system settings");
$self = {};

eval {
	$self = $DBH->run( sub {
		my $arr = $_->selectall_arrayref(
			q{ SELECT * FROM Settings },
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

my $args = {
	'q_location' => '132 BELMONT ST, Cambria, PA',
	'location' => '132 BELMONT ST (CLEARWATER ST & BERNARD ST)',
	'geocodeRegexLoc' => undef,
	'recip' => [
		'4432501272@Sprint_PCS',
	],
	'subject' => '[03] 29B04-Bravo  VEH ACC WITH INJURIES',
	'data' => 'INCIDENT HISTORY DETAIL: 201112-30025


INITIATE: 15:34:41  12/07/11   CALL NUMBER:    201112-30025
ENTRY:    15:34:41             CURRENT STATUS: DISPATCHED
DISPATCH: 15:37:20             ALARM LEVEL:
ONSCENE:                       CASE NUMBER:    201112-30025
CLOSE:


LOCATION: 132 BELMONT ST (CLEARWATER ST & BERNARD ST)
LOCATION NOTE: CERNICS CYCLE WORLD
MAP:
STATION:  03        TYPE:     ACC 29B04-Bravo  VEH ACC WITH INJURIES
BOX AREA: 0308G     PRIORITY: 1
ASSIGNED: C31

15:35:01 *RO, S1, S4
15:36:56 ProQAE vehicles.  2.It\'s not known if chemicals
15:36:56 ProQAE or other hazards are involved.  3.There is
15:36:56 ProQAE no one pinned.  4.No one was thrown from
15:36:56 ProQAE the vehicle.  5.Everyone appears to be
15:36:56 ProQAE completely awake (alert).  6.The type and
15:36:56 ProQAE nature of his injuries are not known.
15:36:56 ProQAE 7.It\'s not known if there is SERIOUS
15:36:56 ProQAE bleeding.
15:36:56 ProQAE Breathing status entry
15:36:56 ProQAE Caller Statement: 2 VEHICLES
15:36:56 ProQAE Number of Patients: 1
15:36:56 ProQAE Age: 3 Years
15:36:56 ProQAE Gender: Male
15:36:56 ProQAE Is the patient conscious?: Yes
15:36:56 ProQAE Is the patient breathing?: Yes
15:36:56 ProQAE Chief complaint: 29
15:36:56 ProQAE Consciousness entry
15:36:56 ProQAE 1.The incident involves multiple
15:36:56 ProQAE applicable.  Caller Statement: 2 VEHICLES.
15:36:56 ProQAE Incidents.  Unknown status/Other codes not
15:36:56 !***Nature changed via ProQAE From:
15:36:56 ProQAE Addition Information
15:36:56 !PA
15:36:56 ProQAE Determinant Code: 29B04
15:36:56 ProQAE You are responding to a patient injured in
15:36:56 ProQAE a traffic incident.  The patient is a
15:36:56 ProQAE 3-year-old male, who is conscious and
15:36:56 ProQAE breathing. Unknown status/Other codes not
15:36:56 ProQAE applicable.  Traffic / Transportation
15:37:00 ProQAE Pre-Arrival Instructions
15:37:00 ProQAE Post Dispatch Instructions
15:37:00 ProQAE codes not applicable.
15:37:08 Waiting call viewed (JLS)
15:37:16 Fire service incident 201112-30025 (IDH)
15:37:18 Police Incident Number 201112-104805 (JLS)
15:37:18 TAC CHANNEL RO ASSIGNED (IDH)
15:37:28 STA9 Available 15:37:28 (IDH)
15:37:30 STA3 Available 15:37:30 (IDH)
15:37:45 REC ANOTHER CALL FROM CARLA HILTABIDEL,  (CRM)
15:37:51 248-2313 SHE WAS NOT INVOLVED -- JUST SAW IT (CRM)
15:37:53 AND CALLED (CRM)
15:38:10 ***REARED COLLISION--3 MALE PTN HEAD INJURY (IDH)
15:38:10 (IDH)',
	'service' => 'sms_send',
	'license' => 'PA110003',
	'inc_no' => '201112-30025',
	'message' => '132 BELMONT ST (CLEARWATER ST & BERNARD ST) STA3 E33 R31 STA9 A92 C31',
	'debug' => '1',
	'geocodeRegexAddr' => undef
};

&log("Initiating SOAP::Lite call");
exit;

my $response;
if (
	$response = SOAP::Lite
		->uri( $self->{'sms'}->{'SMS_RelayURI_Addr'} )
		->proxy
		(
			$self->{'sms'}->{'SMS_RelayProxyAddr'},
			timeout	=> $self->{'sms'}->{'SMS_HTTP_Timeout'} || 10
		)
		->on_fault
		(
			sub
			{
				my ($soap, $res) = @_;
				&main::log("SMS server connection error - " . ( ref $res ? $res->faultstring : $soap->transport->status ) . " - SMS incident notification service failed", E_ERROR);
			}
		)
		->init( $args )
)
{
	unless ( $response->result )
	{
		&main::log("SMS incident notification service HTTP SOAP error: $response $! $@ ", E_ERROR);
		return undef;
	}

	if ( $response =~ /(-?[0-9])\n?(.*)?/m )
	{
		my $res = $1;
		my $msg = $2;

		if ( $res == 1 )
		{
			&main::log("SMS incident notification service successful");
			return 1;
		}
		else
		{
			&main::log("SMS incident notification service encountered errors: $msg", E_ERROR);
			return undef;
		}
	}
	else
	{
		&main::log("SMS incident notification errors - Unable to parse server response: $response", E_ERROR);
		return undef;
	}
}

&log("SMS incident notification process complete");
exit 0;

sub init_db
{
	my $dsn = "dbi:$Config->{database}->{driver}:$Config->{database}->{db_name};" .
	( $Config->{database}->{socket} ?
		"socket=$Config->{database}->{socket};" : "host=$Config->{database}->{host};port=$Config->{database}->{port};"
	);

	&log("Opening database connection => $dsn");

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

		&log("Database connection error: " . $DBI::errstr, E_ERROR);
		die "Database connection error: " . $DBI::errstr;
	}

	&log("Setting default DBIx mode => 'fixup' ");

	unless ( $conn->mode('fixup') )
	{
		&log("Error received when attempting to set default mode on line " . __LINE__ . ": " . $DBI::errstr, E_ERROR);
		die "Unable to set default SQL mode on line " . __LINE__ . ". Fatal.";
	}

	return $conn;
}

sub indexArray
{
    1 while $_[0] ne pop;
    @_-1;
}



sub log
{
    my $msg = shift;
    my $level = shift;
    my ($package, $file, $line) = caller;

    $msg = "[$package:$line] $msg ($$)";

	print STDERR "$msg \n";
}

sub rtrim($)
{
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}

sub ltrim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	return $string;
}

sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}
