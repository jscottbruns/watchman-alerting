#!/usr/bin/perl
use strict;
use File::Tail;
use Config::Tiny;
use DBIx::Connector;
use DBD::mysql;
use Exception::Class::DBI;
use Email::Send;
use Email::Simple::Creator;
  
$| = 1;

BEGIN
{
	push @INC, "/usr/local/watchman-alerting/lib";

	use constant LOG_DIR	=> '/var/log/watchman-alerting';
	use constant CONF_FILE	=> '/etc/watchman.conf';

	use vars qw( $CONF_FILE $DBH $STH $HOST );
}

my $Config;
unless ( $Config = Config::Tiny->read( CONF_FILE ) )
{
	print STDERR "Can't read configuration file: " . Config::Tiny->errstr;
	die "Can't read configuration file: ", Config::Tiny->errstr;
}

my $logfile = $ARGV[0];

unless ( $logfile && -f $logfile )
{
	print STDERR "Unable to initiate logfile monitor service, cannot stat file [$logfile] \n";
	exit 1;	
}

$HOST = `hostname`;
$DBH = &main::init_db;
unless ( $STH = &dbh_prepare )
{
	print STDERR "Error preparing insert statement, can't continue without valid statement handle\n";
	exit 1;
}

$file = File::Tail->new( $logfile );

while ( defined( $line = $file->read ) )
{
    if ( $line =~ /^([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})\s(ERROR|CRITICAL)\s(\[.*?\]?)\s(.*)/ )
    {    	
    	my $timestamp = $1;
    	my $type = $2;
    	my $file = $3;
    	my $error = $4;
    	
    	eval {
	    	$STH->execute( $timestamp, $file, $type, $error )
    	};
    	if ( my $ex = $@ )
    	{
    		print STDERR "DBI execute error: " . ( $ex->can('error') ? $ex->error : $ex ) . "\n";
    	}
    	    	
    	&smtp_alert( "$file $error" ) if $type eq 'CRITICAL';
    }
}

sub smtp_alert
{
	my $message = shift;

	my $mailer = Email::Send->new( {
	    mailer => 'SMTP::TLS',
	    mailer_args => [
	        Host => 'smtp.gmail.com',
	        Port => 587,
	        User => 'watchman-monitor@fhwm.net',
	        Password => 'fhwmwatchman',
	        Hello => 'mail.gmail.com',
	    ]
	} );
	
	my $email = Email::Simple->create(
	    header => [
	        From    => 'watchman-monitor@fhwm.net',
	        To      => 'watchman-monitor@fhwm.net',
	        Subject => "Log Monitor Alert $HOST",
	    ],
	    body => $message,
	);
	
	eval { $mailer->send($email) };
	print STDERR "Error sending email: $@" if $@;	
}

sub dbh_prepare
{
	my $sth;
	
	eval {
		$sth = $DBH->run( sub {
			return $_->prepare(
				qq{
					INSERT INTO ErrorLog
					( Timestamp, LogFile, Level, ErrorText )
					VALUES ( ?, ?, ?, ? )
				}
			);
		} )
	};
	
	if ( my $ex = $@ )
	{
		print STDERR "Database exception received when updating unit history for incident [$self->{dispatch}->{IncidentNo}] - Unable to assign dispatched unit [$_unit] " . $ex->error;
		return undef;
	} 	
	
	return $sth;
}

sub init_db
{
	my $dsn = "dbi:$Config->{database}->{driver}:$Config->{database}->{db_name};" .
	( $Config->{database}->{socket} ?
		"socket=$Config->{database}->{socket};" : "host=$Config->{database}->{host};port=$Config->{database}->{port};"
	);

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
	)
	{
		print STDERR "Database connection error: " . $DBI::errstr;
		die "Database connection error: " . $DBI::errstr;
	}
	
	&log("Setting default DBIx mode => 'fixup' ");

	unless ( $conn->mode('fixup') )
	{
		print STDERR "Error received when attempting to set default mode on line " . __LINE__ . ": " . $DBI::errstr;
	}

	if ( $Config->{database}->{debug} )
	{
		$conn->{Callbacks} =
		{
	    	connected	=> sub
	    	{
	    		my ($_dbh, $_sql, $_attr) = @_;
	    	    print STDERR "[SQL DEBUG] DBI connection established";
	    	    return;
			},
	    	prepare		=> sub
	    	{
				my ($_dbh, $_sql, $_attr) = @_;
				print STDERR "[SQL DEBUG] q{$_sql}";
				return;
			}
		}
	}

	return $conn;
}
