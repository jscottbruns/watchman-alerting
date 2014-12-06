#!/usr/bin/perl
package main;

use strict;
use warnings;

$| = 1;

BEGIN
{
	push @INC, "/usr/local/watchman-alerting/lib";

	use constant DAEMON		=> 'watchman-alerting';
	use constant ROOT_DIR	=> '/usr/local/watchman-alerting';
	use constant LOG_DIR	=> '/var/log/watchman-alerting';
	use constant DEBUG_DIR	=> '/var/log/watchman-alerting';
	use constant LOG_FILE	=> 'watchman-alerting.log';
	use constant TEMP_DIR	=> '/tmp';
	use constant CONF_FILE	=> '/etc/watchman.conf';

	use vars qw( $PATH $TEMP_DIR $DEBUG $DEBUG_DIR $PID_FILE $CONF_FILE $log $DBH $self $LOG_DIR $LICENSE $CONTINUE $CONTINUEPOLL $CONTINUE_RFPOLL %PIDS $ACTIVE_ALERT $ACTIVE_ELAPSED );

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
$DEBUG_DIR = DEBUG_DIR;

use Watchman;
use Watchman::LED_Display;
use Watchman::AlertQueue;
use Watchman::NetAudio;
use Watchman::SMS;
use Watchman::RSS;
use Watchman::Printing;
use Watchman::IO;
use Watchman::RF_Alert;

use POSIX;
use Proc::Daemon;
use Proc::PID::File;
use Proc::ProcessTable;
use Log::Dispatch;
use Log::Dispatch::Screen;
use Log::Dispatch::File;
use File::Spec;
use File::Touch;
use File::Path;
use File::Temp qw( tempfile );
use File::Basename;
use File::Slurp;
use HTML::Entities;
use Config::Tiny;
use DBIx::Connector;
use DBD::mysql;
use Exception::Class::DBI;
use XML::LibXML;
use Date::Calc qw( check_time check_date );
use Date::Parse;
use IO::Socket qw(:DEFAULT :crlf);
use Text::Wrap;
use Text::Template;
use LWP::UserAgent;
use HTTP::Headers;
use HTTP::Message;
use HTTP::Cookies;
use SOAP::Lite;
use Parallel::ForkManager;

sub trim($);
sub rtrim($);
sub ltrim($);

# Create logging/debug/tmp directories if they don't exist
File::Path::make_path( LOG_DIR, { mode => 0777 } ) unless -d LOG_DIR;
File::Path::make_path( DEBUG_DIR, { mode => 0777 } ) unless -d DEBUG_DIR;
File::Path::make_path( TEMP_DIR, { mode => 0777 } ) unless -d TEMP_DIR;

# Touch log file if not exists
File::Touch::touch( File::Spec->catfile( LOG_DIR, LOG_FILE ) ) unless -f File::Spec->catfile( LOG_DIR, LOG_FILE );

die	"Failed to find system configuration file ", CONF_FILE, " - Unable to continue with system startup" unless -f CONF_FILE;
$PID_FILE = `grep "^pidfile=" $CONF_FILE | sed -e 's/pidfile=//g'` || '/var/run/watchman-alerting.pid';

Proc::Daemon::Init( {
	work_dir		=> '/',
	child_STDOUT	=> File::Spec->catfile( LOG_DIR, 'watchman-alerting.stdout'),
	child_STDERR	=> File::Spec->catfile( LOG_DIR, 'watchman-alerting.stderr'),
	pid_file		=> $PID_FILE
} ) unless $0 eq 'watchman-alerting.pl';

if ( Proc::PID::File->running() )
{
	&log("WatchmanAlerting system is already running", E_ERROR);
	print STDERR "WatchmanAlerting system already running";
	exit 1;
}

&init_log;

$SIG{HUP} = sub {
	&log("SIGHUP Received - Setting interupt signal");
	$CONTINUE = $CONTINUEPOLL = $CONTINUE_RFPOLL = 0;

	&term_handler;
#	exit 0;
};

$SIG{INT} = sub {
	&log("SIGHINT Received - Setting interupt signal");
	$CONTINUE = $CONTINUEPOLL = $CONTINUE_RFPOLL = 0;

	&term_handler;
#	exit 0;
};

$SIG{QUIT} = sub {
	&log("SIGQUITE Received - Setting interupt signal");
	$CONTINUE = $CONTINUEPOLL = $CONTINUE_RFPOLL = 0;

	&term_handler;
	exit 0;
};

$SIG{TERM} = sub {
	&log("SIGTERM Received - Sending interupt signal");
	$CONTINUE = $CONTINUEPOLL = $CONTINUE_RFPOLL = 0;

	my $proc = Proc::ProcessTable->new;
	my @ps = map { $_->pid if ($_->cmndline =~ /test-pid.pl/) } @{$proc->table};

	&killchd($_, 9) foreach @ps;
	exit 0;
};

sub killchd ($;$)
{
    my $sig = ($_[1] =~ /^\-?\d+$/) ? $_[1] : 0;
    my $proc = Proc::ProcessTable->new;
    my %fields = map { $_ => 1 } $proc->fields;
    return undef unless exists $fields{'ppid'};
    foreach (@{$proc->table})
    {
    	kill $sig, $_->pid if ($_->ppid == $_[0]);
    };

    kill $sig, $_[0];
};

#$SIG{CHLD} = 'IGNORE'; // Change to default to prevent "no child processes" error
$SIG{CHLD} = 'DEFAULT';

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

if ( $self->{'ledalerting'}->{'LED_DisplayEnabled'} )
{
	&log("Initiating LED displays");

	unless ( &init_ledboards )
	{
		&log("Signboard configuration returned with errors, continuing with limited functionality LED signboards displays", E_ERROR);
	}
}

ALERTQUEUE_START:

if ( $self->{'alertqueue'}->{'AlertQueueEnabled'} )
{
	&log("Initiating AlertQueue Active Incident Polling Service");

	$CONTINUEPOLL = 1;

	if ( ! ( my $p = fork ) )
	{
        my $AlertQueue;

        unless ( $AlertQueue = Watchman::AlertQueue->new )
        {
        	&log("Error initiating AlertQueue polling module - Can't continue with AlertQueue startup", E_CRIT);
        	exit 1;
        }

		my $silent_sleep;
		my $SleepCycle = {
			'Sleep'				=> undef,
			'Wake'				=> undef,
			'Exec_DayOfWeek'	=> [],
			'Exec_Date'			=> [],
			'SilentAlert'		=> undef
		};

		if ( $self->{'alertqueue'}->{'SleepTime'} =~ /([0-9]{4})-([0-9]{4})/ )
		{
			$SleepCycle->{'Sleep'} = $1;
			$SleepCycle->{'Wake'} = $2;

			unless ( check_time( substr($SleepCycle->{'Sleep'}, 0, 2), substr($SleepCycle->{'Sleep'}, 2, 2), 0 ) && check_time( substr($SleepCycle->{'Wake'}, 0, 2), substr($SleepCycle->{'Wake'}, 2, 2), 0 ) )
			{
				&log("Sleep/wake times contain invalid time format. Disabling AlertQueue sleep cycle", E_ERROR);
				undef $SleepCycle->{'Sleep'};
				undef $SleepCycle->{'Wake'};
			}

			if ( defined $SleepCycle->{'Sleep'} && defined $SleepCycle->{'Wake'} )
			{
				&log("AlertQueue sleep cycle active, setting sleep/wake times: [ $SleepCycle->{Sleep}-$SleepCycle->{Wake} ] ") if ( defined $SleepCycle->{'Sleep'} && defined $SleepCycle->{'Wake'} );

                if ( $self->{'alertqueue'}->{'SleepExceptionDayOfWeek'} )
                {
					&log("Setting AlertQueue sleep cycle override by day of week [ $self->{alertqueue}->{SleepExceptionDayOfWeek} ]");
					$SleepCycle->{'Exec_DayOfWeek'} = [ split(',', $self->{'alertqueue'}->{'SleepExceptionDayOfWeek'}) ];
                }

                if ( $self->{'alertqueue'}->{'SleepExceptionDate'} )
                {
	                &log("Setting AlertQueue sleep cycle override by date [ $self->{alertqueue}->{SleepExceptionDate} ]");
	                $SleepCycle->{'Exec_Date'} = [ split(',', $self->{'alertqueue'}->{'SleepExceptionDate'}) ];
                }
    	    }
		}

	    $self->{'alertqueue'}->{'PollErrInterval'} = 5 unless ( $self->{'alertqueue'}->{'PollErrInterval'} );
	    $self->{'alertqueue'}->{'PollInterval'} = 10 unless ( $self->{'alertqueue'}->{'PollInterval'} );

		&log("Entering AlertQueue incident polling loop using [ $self->{alertqueue}->{PollInterval} ] second interval");

	    LOOP_WHILE:
		while ( $CONTINUEPOLL )
		{
			undef $silent_sleep;

            my $dw_i = &main::indexArray( POSIX::strftime('%w', localtime), @{ $SleepCycle->{'Exec_DayOfWeek'} } );
            my $dt_i = &main::indexArray( POSIX::strftime('%m/%d', localtime), @{ $SleepCycle->{'Exec_Date'} } );

			if ( defined $SleepCycle->{'Sleep'} && defined $SleepCycle->{'Wake'} && $dw_i == -1 && $dt_i == -1 )
			{
				my $k = 0;
		        while ( POSIX::strftime('%H%M', localtime) >= $SleepCycle->{'Sleep'} && POSIX::strftime('%H%M', localtime) < $SleepCycle->{'Wake'} )
		        {
					if ( $self->{'alertqueue'}->{'SilentSleepMode'} )
					{
						$silent_sleep = 1;
						&log("AlertQueue Active Incident Polling Sleep Cycle Active - Continuing under silent alert mode ");

						last;
					}
					else
					{
			            &log("AlertQueue Active Incident Polling Sleep Cycle Active - Suspending incident polling") if ( $k++ % 10 == 0 );
			            sleep 180;
					}
				}
			}

			if ( my $result = $AlertQueue->poll )
			{
				if ( $result == 1 )
				{
					&log("Initiating alerting for new active incident");

					if ( $silent_sleep )
					{
						&log("Sleep cycle is currently active with silent alert enabled, proceeding with silent alerting ");
						goto AlertQueueSMS;
					}

					if ( $self->{'audioalerting'}->{'NetAudioEnabled'} )
					{
						if ( ! ( my $audio_fork = fork ) )
						{
							&log("Initiating AlertQueue audible alerting");

			                my $_audio = [];
			                push @{ $_audio }, $AlertQueue->{'call_alert'}->{'PreambleTone'} if $AlertQueue->{'call_alert'}->{'PreambleTone'};
			       			push @{ $_audio }, $AlertQueue->{'call_alert'}->{'TypeAnnounce'} if $AlertQueue->{'call_alert'}->{'TypeAnnounce'};
			       			push @{ $_audio }, $AlertQueue->{'call_alert'}->{'BoxAreaAudio'} if $AlertQueue->{'call_alert'}->{'BoxAreaAudio'};

							Watchman::NetAudio->new(
								IncType		=> 'active',
								AlertClass	=> $AlertQueue->{'alert_controls'}->{'alert_class'},
								BoxArea		=> $AlertQueue->{'call_alert'}->{'BoxArea'},
								AudioSrc	=> $_audio
							);

							exit(0);
						}
					}

					&log("AlertQueue audible alerting disabled") unless $self->{'audioalerting'}->{'NetAudioEnabled'};

					if ( $self->{'ledalerting'}->{'LED_DisplayEnabled'} )
					{
						if ( ! ( my $serial_pid = fork ) )
						{
		                    my $ttylock = &main::ttylock;

							&log("Locking serial TTY device [$ttylock]");

	                        unless (
		                        Watchman::LED_Display->new(
		                            'xml_msg'       =>  $AlertQueue->{'serial_message'}
	                            )
	                        )
	                        {
	                        	&log("Error received during LED display processing", E_ERROR);
							}

				            if ( $ttylock )
				            {
				            	$self->{'alertqueue'}->{'AlertQueueLED_DisplayReset'} = $self->{'alertqueue'}->{'AlertQueueLED_DisplayReset'} || 120;

				            	&log("Pausing for [$self->{alertqueue}->{AlertQueueLED_DisplayReset}] seconds for LED display reset");
					            sleep $self->{'alertqueue'}->{'AlertQueueLED_DisplayReset'};

								my $current_ttylock;
								&log("Checking for valid TTY lock [$ttylock]");

								$current_ttylock = read_file( File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) if ( -f File::Spec->catfile( $PATH, 'tty_lock.lock' ) );
								if ( $current_ttylock && $current_ttylock == $ttylock )
								{
									&log("TTY lock is active, resetting LED displays");

						            if (
						                Watchman::LED_Display->new(
						                	'reset'		=> 1
						                )
						            )
						            {
						                &log("LED display reset successful");
						            }
									else
									{
						                &log("TTY display error during LED display reset - Unable to reset signboard displays", E_ERROR);
						            }
								}
								else
								{
									&log("TTY lock expired [$current_ttylock] - LED display reset not required");
								}
				            }

					        exit 0;
						}

					} else { &log("Serial LED alerting is disabled"); }

			        if ( $self->{'io'}->{'IO_Enabled'} )
			        {
			        	Watchman::IO->new( {
			        		Units		=> []
			        	} );
			        }

			        &log("IO device triggers have been disabled") unless ( $self->{'io'}->{'IO_Enabled'} );

					AlertQueueSMS: # Jump to label when sleep active and silent flag set

	                if ( $self->{'sms'}->{'SMS_Enabled'} && defined $AlertQueue->{'sms'} )
	                {
	                	if ( ! ( my $preinc_notify_fork = fork ) )
	                	{
							&log("Preparing SMS notifications");

			                unless (
				                Watchman::SMS->new(
				                    'AlertType'	=> 'active',
			                        'CallNo'	=> $AlertQueue->{'call_alert'}->{'EventNo'},
			                        'Subject'	=> $AlertQueue->{'sms'}->{'subject'},
			                        'Message'	=> $AlertQueue->{'sms'}->{'message'},
			                        'BoxArea'	=> $AlertQueue->{'call_alert'}->{'BoxArea'},
			                        'Location'	=> $AlertQueue->{'call_alert'}->{'Location'},
			                        'QLocation'	=> $AlertQueue->{'call_alert'}->{'Location'},
			                        'CallType'	=> $AlertQueue->{'call_alert'}->{'CallType'},
			                        'Nature'	=> $AlertQueue->{'call_alert'}->{'Nature'},
			                        'Label'		=> $AlertQueue->{'call_alert'}->{'Label'}
							) )
							{
				                &log("AlertQueue SMS notifications failed with errors", E_ERROR);
								exit (1);
			                }

							&log("Pending incident notifications completed");
				            exit (0);
						}
					} else { &log("SMS incident notification service has been disabled"); }

				}
				elsif ( $result == -2 )
				{
					&log("Fatal response sent by CAD pending incident listener, pending incident alerting unable to continue", E_ERROR);
		            last LOOP_WHILE;
		        }

	            &log("AlertQueue HTTP Request Error # $AlertQueue->{req_err}", E_ERROR) if $AlertQueue->{'req_err'} > 0;
    	    }

    	    sleep $self->{'alertqueue'}->{'PollInterval'};
		}

		&log("Interupt signal received, stopping AlertQueue Active Incident Polling Service");

		exit (0);
	}
}

&log("AlertQueue Active Incident Polling Service is disabled") unless ( $self->{'alertqueue'}->{'AlertQueueEnabled'} );

my $FORK = Parallel::ForkManager->new(5);

$FORK->run_on_finish( sub
{
	my ($pid, $retval, $ident) = @_;

	if ( ( $ident eq 'RF_Alert' && $CONTINUE_RFPOLL ) || ( $ident eq 'INET' && $CONTINUE ) )
	{
		&log("Premature terminiation ($retval) of $ident process, attempting to restart...", E_CRIT);
		goto RFTONE_START if $ident eq 'RF_Alert';
		goto INET_START if $ident eq 'INET';
	}
} );

if ( $self->{'rfalert'}->{'RF_Alerting_Enabled'} )
{
	$self->{'rfalert'}->{'Poll_Interval'} = 500 if ( ! $self->{'rfalert'}->{'Poll_Interval'} || $self->{'rfalert'}->{'Poll_Interval'} < 100 );
	$CONTINUE_RFPOLL = 1;

	RFTONE_START:
	if ( ! ( my $rfpid = $FORK->start('RF_Alert') ) )
	{
        my $RF_Alert;

        unless ( $RF_Alert = Watchman::RF_Alert->new )
        {
        	&log("Error initiating RF Alert module - Can't continue with RF tone alerting", E_CRIT);
        	$FORK->finish(1);
        }

		&log("Starting RF tone channel polling...");

		RFTONE_LOOP:
        while ( $CONTINUE_RFPOLL )
        {
			if ( my $rf_res = $RF_Alert->poll_amqp )
			{
				if ( ref $rf_res eq 'HASH' && $rf_res->{'ToneId'} > 0 )
				{
					&log("RF tone activation received on Tone #" . $rf_res->{'ToneId'} . ", initiating static alerting");

					if ( $self->{'audioalerting'}->{'NetAudioEnabled'} )
					{
						if ( ! ( my $audio_fork = fork ) )
						{
							&log("Initiating audible alerting for RF tone activation");

			                my $_audio = [];
			                push @{ $_audio }, $rf_res->{'AudioPreamble'} || $self->{'audioalerting'}->{'DefaultAudioPreamble'};
			       			push @{ $_audio }, $rf_res->{'AudioAnnouncement'} if $rf_res->{'AudioAnnouncement'};

							Watchman::NetAudio->new(
								IncType		=> 'rftone',
								ToneID		=> $rf_res->{'ToneId'},
								AudioSrc	=> $_audio
							);

							exit(0);
						}
					}

					&log("Audible alerting disabled") unless $self->{'audioalerting'}->{'NetAudioEnabled'};

					if ( $self->{'ledalerting'}->{'LED_DisplayEnabled'} && $RF_Alert->{'led_display'} )
					{
						if ( ! ( my $led_pid = fork ) )
						{
		                    my $ttylock = &main::ttylock;

							&log("Locking LED display device [$ttylock]");

	                        unless (
		                        Watchman::LED_Display->new(
		                            'xml_msg'       =>  $RF_Alert->{'led_display'}
	                            )
	                        )
	                        {
	                        	&log("Error received during LED display processing", E_ERROR);
							}

				            if ( $ttylock )
				            {
				            	&log("Pausing for [$RF_Alert->{RF_LEDResetTime}] seconds for LED display reset");
					            sleep $RF_Alert->{'RF_LEDResetTime'};

								my $current_ttylock;
								&log("Checking for valid TTY lock [$ttylock]");

								$current_ttylock = read_file( File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) if ( -f File::Spec->catfile( $PATH, 'tty_lock.lock' ) );
								if ( $current_ttylock && $current_ttylock == $ttylock )
								{
									&log("TTY lock is active, resetting LED displays");

						            if (
						                Watchman::LED_Display->new(
						                	'reset'		=> 1
						                )
						            )
						            {
						                &log("LED display reset successful");
						            }
									else
									{
						                &log("TTY display error during LED display reset - Unable to reset signboard displays", E_ERROR);
						            }
								}
								else
								{
									&log("TTY lock expired [$current_ttylock] - LED display reset not required");
								}
				            }

					        exit 0;
						}

					}
					else
					{
						&log("LED alerting is disabled");
					}

			        if ( $self->{'io'}->{'IO_Enabled'} )
			        {
			        	Watchman::IO->new( {
			        		ToneID		=> [ $rf_res->{'ToneId'} ]
			        	} );
			        }

			        &log("IO device triggers have been disabled") unless $self->{'io'}->{'IO_Enabled'};



				}
				elsif ( $rf_res < 0 )
				{
					&log("Error response received from RF alert module ($rf_res) - Aborting RF tone alerting", E_CRIT);
		        }
			}

			select(undef, undef, undef, $self->{'rfalert'}->{'Poll_Interval'});
        }

        $FORK->finish(0);
	}
}

if ( $self->{'inet'}->{'INET_Enabled'} )
{
	&log("iCAD dispatch service enabled, initiating iCAD remote network daemon");

	$CONTINUE = 1;

	INET_START:
	if ( ! ( my $inetpid = $FORK->start('INET') ) )
	{

		my $sock = IO::Socket::INET->new(
			LocalAddr	=> $self->{inet}->{LocalAddr} || '0.0.0.0',
			LocalPort	=> $self->{inet}->{LocalPort} || 8888,
			Listen		=> SOMAXCONN,
			Proto		=> $self->{inet}->{Protocol} || 'tcp',
			Blocking	=> 0,
			Type		=> SOCK_STREAM,
			Reuse		=> 1,
			Timeout		=> 5
		);

		unless ( $sock )
		{
			&log("Error initiating iCAD dispatch client - $@ - Fatal", E_CRIT);
			$FORK->finish(1);
		}

		&log("Listening for iCAD data connections on " . $sock->sockhost() . ":" . $sock->sockport());


		while ( $CONTINUE )
		{
			my $client = $sock->accept();
			next unless ( $client );

			if ( ! ( my $p =  fork ) )
			{
				$sock->close;

				{
					local $/ = CRLF . CRLF;
					$client->autoflush(1);

					if ( chomp( $self->{'StreamData'} = <$client> ) )
					{
						&log("Incoming data stream (" . length($self->{'StreamData'}) . " bytes)");

						if ( $DEBUG )
						{
							my ($fh, $filename) = tempfile(
								'iCADDispatch_XXXXXX',
								DIR		=> LOG_DIR,
								SUFFIX	=> '.xml'
							);

							&log("Writing input stream to temp file => $filename ", E_DEBUG);
							print $fh $self->{'StreamData'};
							close $fh;
						}

						if ( ltrim( $self->{'StreamData'} ) =~ /<fhwm:ICadDispatch/m )
						{
							&log('Processing iCAD dispatch payload');

							my $xp;
							eval
							{
								$xp = XML::LibXML->load_xml( string => $self->{'StreamData'} )
							};

							if ( $@ )
							{
								&main::log("XML parsing error while attempting to read alert transaction identifier $! $@", E_CRIT);
							}

							if ( $xp )
							{
								if ( $xp->findnodes('//ns4:MessageSequenceNumber/ns3:IdentificationID') )
								{
									my $TransID = $xp->findnodes('//ns4:MessageSequenceNumber/ns3:IdentificationID')->to_literal;
									my $SubmitTime = $xp->findnodes('//ns4:SubmissionDateTime/ns3:DateTime')->to_literal;

									&log("iCAD Transaction => [$TransID] Timestamp => [$SubmitTime]");

									my $LocalID;
									eval {
										$LocalID = $DBH->run( sub {
											$_->do(
												qq{
													INSERT INTO iCADAlertTrans
													( iCADTransID, SentTime, StreamData )
													VALUES ( ?, ?, ? )
												},
												undef,
												$TransID,
												$SubmitTime,
												$self->{'StreamData'}
											);
											return $_->selectrow_arrayref('SELECT LAST_INSERT_ID() AS ID');
										} )
									};

									if ( my $ex = $@ )
									{
										&log("Database exception received while attempting to store iCAD transaction identifier: " . $ex->error, E_ERROR);
									}

									&log("Returning Client Transaction ID => [ $TransID:$LocalID->[0] ]");

									print $client "$TransID:$LocalID->[0]" . CRLF;

								}
							}
						}
					}
				}

				close $client;

				if ( length( $self->{'StreamData'} ) > 0 )
				{
					&log("Submitting iCAD incident payload for alert processing");
					&alert( $self->{'StreamData'} );
				}

				exit(0);
			}
			elsif ( $p )
			{
				close $client;
			}

			&log("Interupt signal received, stopping iCAD remote network daemon") unless $CONTINUE;
		}

		&log("Closing INET socket");
		$sock->close;

		$FORK->finish(0);
	}
}

&log("INET network dispatch service is disabled") unless $self->{'inet'}->{'INET_Enabled'};

$FORK->wait_all_children;

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

	if ( $Config->{database}->{debug} )
	{
		&log("Registering database handler callback debugging functions");
		$conn->{Callbacks} =
		{
	    	connected	=> sub
	    	{
	    		my ($_dbh, $_sql, $_attr) = @_;
	    	    &log("[SQL DEBUG] DBI connection established", E_ERROR);
	    	    return;
			},
	    	prepare		=> sub
	    	{
				my ($_dbh, $_sql, $_attr) = @_;
				&log("[SQL DEBUG] q{$_sql}", E_ERROR);
				return;
			}
		}
	}

	return $conn;
}


sub init_ledboards
{
	unlink File::Spec->catfile( $PATH, 'default.xml') if -f File::Spec->catfile( $PATH, 'default.xml');

	my $defaultInit;

	$self->{'ledalerting'}->{'LED_DefaultTextPage'} = 'A' unless $self->{'ledalerting'}->{'LED_DefaultTextPage'};
	my $defaultTextSize = $self->{'ledalerting'}->{'LED_DefaultTextSize'} || '768';

	unless ( $self->{'ledalerting'}->{'LED_DefaultTextPage'} eq 'A' )
	{
		&log("Configuring display address space [$self->{ledalerting}->{LED_DefaultTextPage}]");
		$defaultInit = "<memoryConfig><textConfig label=\"$self->{ledalerting}->{LED_DefaultTextPage}\" size=\"$defaultTextSize\" /></memoryConfig>";
	}

	unless (
		Watchman::LED_Display->new(
			'reset'		=> 1,
			'xml_msg'	=> $defaultInit
		)
	) {
		&log("Errors received during LED display initialization", E_ERROR);
		return undef;
	}

    &log("LED display initiation complete");

    return 1;
}


sub alert
{
	my $stream = shift;

    my ($alert, $ttytimer, $ttylock);

    if ( $alert = Watchman->new( 'data' => $stream ) )
    {
        if ( $alert->{'alert'} )
        {
			&log("Alert flag set - Initiating incident alerting");

	        if ( ! $alert->{'dispatch'}->{'silent'} && $self->{'audioalerting'}->{'NetAudioEnabled'} && @{ $alert->{'dispatch'}->{'audiosrc'} } )
	        {
				if ( ! ( my $audio_fork = fork ) )
				{
					&log("Initiating incident audible alerting");

					Watchman::NetAudio->new(
						IncType		=> 'assigned',
						Units		=> $alert->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'},
						AudioSrc	=> $alert->{'dispatch'}->{'audiosrc'},
						Priority	=> $alert->{'dispatch'}->{'priority'}
					);

					&log("Audible alerting complete");

					exit 0;
				}
			}

			&log("Audible alerting is disabled") unless ( $self->{'audioalerting'}->{'NetAudioEnabled'} );
	        &log("Silent alert flag is set - Skipping audible alerting") if $alert->{'dispatch'}->{'silent'};

	        if ( $self->{'ledalerting'}->{'LED_DisplayEnabled'} )
	        {
	        	if ( ! ( my $serial_pid = fork ) )
	        	{
			        $ttylock = &main::ttylock;
		            &log("Locking serial TTY device [$ttylock]");

			        # LED preamble message display
					unless ( $self->{'ledalerting'}->{'LED_SkipPreambleDisplay'} )
					{
			            if (
			                Watchman::LED_Display->new(
			                    'xml_msg'	=> $alert->{'dispatch'}->{'LED_Message'}->{'title'}
			                )
			            ) {
			                $self->{'ledalerting'}->{'LED_PreambleLength'} = 3 unless ( $self->{'ledalerting'}->{'LED_PreambleLength'} );

			                &log("Sucessfully sent LED preamble display, $self->{ledalerting}->{LED_PreambleLength} second wait time to scroll message display");
			                sleep $self->{'ledalerting'}->{'LED_PreambleLength'};
			            }
			            else
			            {
							&log("Serial display error during LED preamble display - Skipping preamble wait period", E_ERROR);
			            }
	        		}

	        		&log("Skip LED preamble flag set, skipping LED preamble ") if $self->{'ledalerting'}->{'LED_SkipPreambleDisplay'};

					# LED scroll message display
		            if (
		                Watchman::LED_Display->new(
		                    'xml_msg'   => $alert->{'dispatch'}->{'LED_Message'}->{'message'}
		                )
		            ) {
		                $ttytimer = $self->{'ledalerting'}->{'LED_MessageResetTime'} || 120;
		                &log("LED scroll message display successful - $ttytimer second wait time to LED display reset");
		            }
					else
					{
		                &log("Serial display error during LED scroll message display. LED signboard alerting failed", E_ERROR);

		                $ttytimer = 1;
		                undef $ttylock;;
		                &main::ttylock_reset;
		            }

		            &log("LED signboard alerting complete");

		            if ( $ttytimer )
		            {
			            sleep $ttytimer;

						my $current_ttylock;
						&log("Checking for valid TTY lock [$ttylock]");

						$current_ttylock = read_file( File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) if ( -f File::Spec->catfile( $PATH, 'tty_lock.lock' ) );
						if ( $current_ttylock && $current_ttylock == $ttylock )
						{
			            	&log("TTY lock is active, Starting LED display reset with default message [ " . File::Spec->catfile( $PATH, "default.xml" ) . "]");

				            if (
				                Watchman::LED_Display->new(
				                    'reset'	=> 1
				                )
				            )
				            {
				                &log("LED display reset successful");
				            }
							else
							{
				                &log("TTY display error during LED display reset - Unable to reset signboard displays", E_ERROR);
				            }
						}
						else
						{
							&log("TTY lock expired - LED display reset not required");
						}
		            }

		            exit 0;
	        	}
	        }

	        &log("LED signboard alerting has been disabled") unless ( $self->{'ledalerting'}->{'LED_DisplayEnabled'} );

	        if ( $self->{'io'}->{'IO_Enabled'} )
	        {
	        	Watchman::IO->new( {
	        		Units		=> $alert->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'},
	        	} );
	        }

	        &log("IO device triggers have been disabled") unless ( $self->{'io'}->{'IO_Enabled'} );
        }

	    if ( $self->{'sms'}->{'SMS_Enabled'} )
	    {
	    	&log("Initiating SMS incident notifications");

            if ( ! ( my $notify_pid = fork ) )
			{
				unless (
	            	Watchman::SMS->new(
	                	'Units'				=> $alert->{'dispatch'}->{'UnitDispatch'}->{'UnitList'},
	                    'MyUnits'			=> $alert->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'},
	                    'BoxArea'			=> $alert->{'dispatch'}->{'Station'},
	                    'Location'			=> $alert->{'dispatch'}->{'Location'}->[0],
	                    'CrossSt'			=> $alert->{'dispatch'}->{'CrossStreet'},
	                    'GeocodeRegexAddr' 	=> $alert->{'dispatch'}->{'geocodeRegexAddr'},
	                    'GeocodeRegexLoc'  	=> $alert->{'dispatch'}->{'geocodeRegexLoc'},
	                    'CallGroup'			=> $alert->{'dispatch'}->{'CallGroup'},
	                    'CallType'			=> $alert->{'dispatch'}->{'CallType'},
	                    'CallLabel'         => $alert->{'dispatch'}->{'CallLabel'},
	                    'Narrative'			=> $alert->{'dispatch'}->{'IncidentNotes'},
	                    'IncidentNo'		=> $alert->{'dispatch'}->{'IncidentNo'},
	                    'CallNo'			=> $alert->{'dispatch'}->{'CallNo'},
	                    'Printout'			=> $alert->{'dispatch'}->{'printout'}
				) )
	            {
	            	&log("Errors encountered during SMS incident notification process", E_ERROR);
	                exit 1;
				}

				&log("SMS incident notification process complete");
	            exit 0;
			}
	    }

	    &log("SMS incident notifications disabled") unless ( $self->{'sms'}->{'SMS_Enabled'} );

	    if ( $self->{'rss'}->{'RSS_Enabled'} )
	    {
			if ( ! ( my $rss_pid = fork ) )
			{
		        &log("Initiating RSS incident feed service");
	            unless (
		            Watchman::RSS->new(
	                	'Units'			=> $alert->{'dispatch'}->{'UnitDispatch'}->{'UnitList'},
		                'BoxArea'		=> $alert->{'dispatch'}->{'BoxArea'},
	                    'Location'		=> $alert->{'dispatch'}->{'Location'},
	                    'CallGroup'		=> $alert->{'dispatch'}->{'CallGroup'},
	                    'CallType'		=> $alert->{'dispatch'}->{'CallGroup'},
	                    'CallLabel'		=> $alert->{'dispatch'}->{'Label'},
	                    'Text'			=> $alert->{'dispatch'}->{'IncidentNotes'},
	                    'IncidentNo'	=> $alert->{'dispatch'}->{'IncidentNo'},
	                    'CallNo'		=> $alert->{'dispatch'}->{'CallNo'}
				) )
	            {
	            	&log("Errors encountered during RSS incident feed update process", E_ERROR);
	                exit 1;
				}

	            &log("RSS feed update complete");
	            exit 0;
			}
	    }

	    if ( $self->{'printing'}->{'RemotePrintingEnabled'} )
	    {
	    	if ( ! ( my $printpid = fork ) )
	    	{
		    	&log("Initiating remote printing system");

				unless (
					Watchman::Printing->new(
						'IncidentNo'	=> $alert->{'dispatch'}->{'IncidentNo'},
						'StreamData'	=> $self->{'StreamData'},
						'Printout'		=> $alert->{'dispatch'}->{'printout'}
					)
				)
				{
					&log("Errors encountered during fire station printing", E_ERROR);
					exit 1;
				}

				&log("Fire station printing complete");
				exit 0;
	    	}
	    }

        undef $self->{'StreamData'};
        undef $alert;

		# 12/20 - Removing child pid cleanup due to >95% CPU usage during wait period
        # &log("Reaping child processes");
        # my $ret;
        # do {
        #     $ret = waitpid(-1, &WNOHANG);
	    #     &log("Child $ret exited with retval $?") if ( $ret != 0 && $ret != -1 );
        # } until $ret == -1;
		#
        # &log("All child processes have been reaped");
		#
        # &log("Incident alerting complete");

    }
    else
    {
		if ( $self->{'printing'}->{'RemotePrintingEnabled'} )
		{
	    	if ( ! ( my $printpid = fork ) )
	    	{
		    	&log("Initiating remote printing system");

				unless (
					Watchman::Printing->new(
						'StreamData'	=> $self->{'StreamData'}
					)
				)
				{
					&log("Errors encountered during fire station printing", E_ERROR);
					exit 1;
				}

				&log("Fire station printing complete");
				exit 0;
	    	}
		}
    }

    undef $self->{'StreamData'} if $self->{'StreamData'};
}

sub log
{
    my $msg = shift;
    my $level = shift;
    my ($package, $file, $line) = caller;

	$level = E_INFO if ! $level;

    $msg = "[$package:$line] $msg ($$)";

	$log->$level($msg) if defined $log;
	print STDERR "$msg \n" unless defined $log;
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

sub term_handler
{
    &log("Reaping remaining child processes");

    kill 15, $_ for ( keys %PIDS );

    my $ret;
    do {
        $ret = waitpid(-1, &WNOHANG);
        &log("Child $ret exited with retval $?") if ( $ret != 0 && $ret != -1 );
    } until $ret == -1;

    &log("Done cleaning up child processes");
}

sub ttylock
{
    my $tty_id = int( rand(50000) ) + 5000;

    if ( open(TTY_LOCK, '>', File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) )
    {
    	print TTY_LOCK $tty_id;
    	close TTY_LOCK;
    }

    return $tty_id;
}

sub ttylock_reset
{
    if ( open(TTY_LOCK, '<', File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) )
    {
    	truncate TTY_LOCK, 0;
	    close TTY_LOCK;
    }
}

sub ttylock_read
{
    if ( open(TTY_LOCK, '<', File::Spec->catfile( $PATH, 'tty_lock.lock' ) ) )
    {
	    my $lock = <TTY_LOCK>;
	    close TTY_LOCK;

	    return $lock;
    }

    return undef;
}

sub indexArray
{
    1 while $_[0] ne pop;
    @_-1;
}

sub init_connect
{
	my $tmpfile = shift;

	if ( $tmpfile && -f $tmpfile ) {

		&log("Calling Watchman::Connect for data relay to $self->{soap_proxy}/$self->{soap_uri} \n");
		unless (
			Watchman::Connect->new(
				'datafile'		=>	$tmpfile,
				'uri'			=>	'Watchman/SOAPTEST', # $self->{'soap_uri'},
				'proxy'			=>	$self->{'soap_proxy'},
				'timeout'		=>	$self->{'soap_timeout'}
			)
		) {

			&log("Errors encountered while talking to Watchman::Connect server, Message delivery failed \n", 1);
			return undef;
		}

		return 1;
	}

	&log("Data package file name not specified, unable to relay message\n", 1);
	return undef;
}

sub is_empty
{
	my $array = shift;

	foreach ( @{ $array } ) {
		return undef if ( $_ );
	}

	return 1;
}

sub init_log
{
	if ( $log = Log::Dispatch->new )
	{
		if ( $0 eq 'watchman-alerting.pl' )
		{
			unless (
				$log->add(
					Log::Dispatch::Screen->new(
						name		=> 'screen',
						min_level	=> 'debug',
						callbacks	=> sub {
							my %h = @_;
							return uc ( $h{level} ) . " $h{message} \n";
						}
					)
				)
			)
			{
				print STDERR "Error appending system logging to console output $! $@ \n";
			}
		}

		unless (
			$log->add(
				Log::Dispatch::File->new(
					name		=> 'file',
					min_level	=> 'debug',
					mode		=> 'append',
					filename	=> File::Spec->catfile( LOG_DIR, LOG_FILE ),
					callbacks	=> sub {
						my %h = @_;
						return strftime("%Y-%m-%d %H:%M:%S", localtime()) . " " . uc ( $h{level} ) . " $h{message} \n";
					}
				)
			)
		)
		{
			print STDERR "Error appending system logging to log file output $! $@ ($$) \n";
		}

		return 1;
	}

	print STDERR "Unable to initiate system logging $! $@ ($$) \n";
	return undef;
}

sub userinput
{
    print "Press enter to continue...";
    chomp( my $u = <> );

    die if $u eq 'q';
    return;
}

sub END
{

}

sub regex_format
{
	my $text = shift;
	my $regex = shift;

	my @patterns = split(/\n/, $regex);
	my @subs;

	foreach my $pattern ( @patterns )
	{
		$pattern =~ s/^\///;
		$pattern =~ s/\/$//;

		my ($match, $replace) = split(/\//, $pattern);

		if ( $match && $replace )
		{
			push @subs,
			{
				'match'		=> $match,
				'replace'	=> $replace
			};
		}
	}

	foreach my $patt ( @subs )
	{
		$text =~ s/$patt->{match}/"\"$patt->{replace}\""/ee;
	}

	return $text;
}

sub hex2bin
{
    my ($hex_string, $ret_array) = @_;

	my @hex_digits = (0..9, 'A'..'F');
	my $valid_hex = join '', @hex_digits;
	my %hex2binary;

	for ( 0..15 )
	{
	    $hex2binary{$hex_digits[$_]} = ($_>>3).($_%8>>2).($_%4>>1).$_%2;
	}

    return unless $hex_string =~ /^[$valid_hex]+$/i;
    $hex_string =~ s/([$valid_hex])/$hex2binary{uc $1}/gi;

    return $hex_string unless $ret_array;
    return reverse( split('', $hex_string ) );
}

sub bin2hex
{
	my $binary = shift;

	return unless $binary;

	my $int = unpack("N", pack("B32", substr("0" x 32 . $binary, -32)));
	return sprintf("%x", $int );
}