package Watchman::RF_Alert;

use POSIX;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use IPC::System::Simple qw(capture $EXITVAL EXIT_ANY);
use IO::Socket qw(:DEFAULT :crlf);
use Modbus::Client;
use FileHandle;
use Socket;
use Time::HiRes qw/ time sleep /;
use Net::RabbitMQ;

sub new
{
    my $this = shift;
    my $params = shift;

	&main::log("Initiating Watchman::RF_Alert Tone Alert Module");

    my $class = ref($this) || $this;
    my $self = {
    	'path'				=> $::PATH,
    	'debug'				=> $::DEBUG,
    	'temp_dir'			=> $::TEMP_DIR,
    	'dbh'				=> $::DBH,
    	'main'				=> $::self,
    	'log_dir'			=> $::LOG_DIR,
    	'Channel_Count'		=> 0,
    	'RF_Channel'		=> {},
    	'ActiveWait'		=> {},
    	'IO_Error'			=> {}
    };

	$self->{'settings'} = {
		'system'		=> $::self->{'system'},
		'rfalert'		=> $::self->{'rfalert'},
		'ledalerting'	=> $::self->{'ledalerting'},
		'audioalerting'	=> $::self->{'audioalerting'},
		'voicealert'	=> $::self->{'voicealert'}
	};

    $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} = $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} || 'xsltproc %xsl_file %source_file';
	$self->{'settings'}->{'system'}->{'DefaultXSL_Path'} = File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'system'}->{'DefaultXSL_Path'} ) if $self->{'settings'}->{'system'}->{'DefaultXSL_Path'};

    $self->{'settings'}->{'ledalerting'}->{'LED_DefaultDisplayColor'} = $self->{'settings'}->{'ledalerting'}->{'LED_DefaultDisplayColor'} || 'red';
	$self->{'settings'}->{'ledalerting'}->{'LED_PreambleDisplayMode'} = $self->{'settings'}->{'ledalerting'}->{'LED_PreambleDisplayMode'} || 'flash';
	$self->{'settings'}->{'ledalerting'}->{'LED_StandardDisplayMode'} = $self->{'settings'}->{'ledalerting'}->{'LED_StandardDisplayMode'} || 'rotate';

    $self->{'settings'}->{'rfalert'}->{'Poll_Interval'} = 500 unless $self->{'settings'}->{'rfalert'}->{'Poll_Interval'};
    $self->{'settings'}->{'rfalert'}->{'Poll_Interval'} = 100 if $self->{'settings'}->{'rfalert'}->{'Poll_Interval'} < 100;
	$self->{'settings'}->{'rfalert'}->{'Poll_Interval'} /= 1000;

	&main::log("Setting polling interval to [$self->{settings}->{'rfalert'}->{Poll_Interval}] second(s)");

	$self->{'settings'}->{'rfalert'}->{'ActiveWait'} = 750 unless $self->{'settings'}->{'rfalert'}->{'ActiveWait'};
	$self->{'settings'}->{'rfalert'}->{'ActiveWait'} = 100 if $self->{'settings'}->{'rfalert'}->{'ActiveWait'} < 100;

	&main::log("Setting active wait time to [$self->{settings}->{rfalert}->{ActiveWait}] ms");

	$self->{'settings'}->{'rfalert'}->{'RF_LEDResetTime'} = 180 unless $self->{'settings'}->{'rfalert'}->{'RF_LEDResetTime'};

	&main::log("Loading active RF tone channels for IO polling");

	my $sth;
	eval {
		$sth = $self->{'dbh'}->run( sub {
			my $sth = $_->prepare( qq{
				SELECT t1.*
				FROM RF_Alert t1
			} );
			$sth->execute;
			$sth;
		} )
	};

	if ( my $ex = $@ )
	{
		&main::log("Database exception received while loading RF tone channels - Unable to proceed with tone alerting" . $ex->error, E_ERROR);
		return undef;
	}

	while ( my $rowref = $sth->fetchrow_hashref )
	{
		$self->{'Channel_Count'}++;

		$self->{'RF_Channel'}->{ $rowref->{'IO_Slot'} }->{ $rowref->{'IO_Channel'} } = $rowref;
	}

	unless ( $self->{'Channel_Count'} > 0 )
	{
		&main::log("No tone channel assignements found, exiting RF_Alerting module");
		return undef;
	}

	&main::log("Finished loading RF tone channel assignments");

	bless $self, $class;

	return $self;
}


sub poll_amqp
{
	my $self = shift;

	my $queue = 'rfalert_queue';

	&main::log("Initiating AMQP RF alert polling ", E_DEBUG) if $self->{'debug'};

	&main::log("Opening connection to AMQP server", E_WARN);

	my $mq = Net::RabbitMQ->new();

	eval {
		$mq->connect("localhost", { user => "guest", password => "guest" })
	};

	if ( $@ )
	{
		&main::log("Unable to connect to AMQP server. Unable to continue RF alert polling", E_CRIT);
		return undef;
	}

	&main::log("Opening AMQP channel ", E_WARN);

	eval {
		$mq->channel_open(1)
	};

	if ( $@ )
	{
		&main::log("Error opening AMQP channel %@", E_CRIT);
		return undef;
	}

	&main::log("Starting AMQP consumer on queue [$queue]", E_WARN);

	eval {
		$mq->consume(1, $queue)
	};

	if ( $@ )
	{
		&main::log("Error starting AMQP consumer on queue [$queue] %@", E_CRIT);
		return undef;
	}

	&main::log("Polling for AMQP messages ", E_WARN);

	while ( 1 )
	{
		if ( my $rfmsg = $mq->recv() )
		{
			my ($slot, $rfchan);
			my $msgbody = $rfmsg->{'body'};
			my $delivery_tag = $rfmsg->{'delivery_tag'};
			my $rf_slots = join '', keys %{ $self->{RF_Channel} };

			if ( $msgbody =~ /^([$rf_slots]{1}):([0-9]{1,})$/ )
			{
				$slot = $1;
				$rfchan = $2;
			}

			&main::log("RF activation ($msgbody) received on slot [$slot] channel [$rfchan]. Checking RF channel mapping.", E_WARN);

			if ( $self->{'RF_Channel'}->{ $slot }->{ $rfchan } )
			{
				&main::log("RF activation mapped to active RF channel assignment");

				$self->{'led_display'} = undef;

				$self->{'led_display'} = "<" .
				( $self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'DisplayColor'} ?
					$self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'DisplayColor'} : 'green'
				) . " />$self->{RF_Channel}->{ $slot }->{ $rfchan }->{DisplayLabel} " if $self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'DisplayLabel'};

				&main::log("Writing RF tone activation assignment(s) to database");

				eval {
					$self->{'dbh'}->run( sub {
						$_->do(
							qq{
								INSERT INTO RF_Incident
								VALUES ( NULL, NULL, ? )
							},
							undef,
							$self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'ToneId'}
						);
					} )
				};

				if ( my $ex = $@ )
				{
					&main::log("Database exception received when saving RF tone activation address to FR incident table - Unable to assign RF tone ID to incident [$self->{RF_Channel}->{ $slot }->{ $rfchan }->{ToneId}] " . $ex->error, E_ERROR);
				}

				&main::log("Acknowledging RF alert message [$delivery_tag]", E_WARN);

				eval {
					$mq->ack(1, $delivery_tag)
				};

				if ( $@ )
				{
					&main::log("Error acknoledging message [$delivery_tag] $@", E_ERROR);
				}

				return {
					'ToneId'			=> $self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'ToneId'},
					'AudioPreamble'		=> $self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'AudioPreamble'},
					'AudioAnnouncement'	=> $self->{'RF_Channel'}->{ $slot }->{ $rfchan }->{'AudioAnnouncement'},
					'LED_Display'		=> $self->{'led_display'}
				};
			}
			else
			{
				&main::log("Unable to map RF activation on slot [$slot] channel [$rfchan] to active channel assignment. Check RF alert configuration", E_ERROR);
				return -1;
			}
		}
	}

	return 1;
}

sub poll
{
	my $self = shift;

	&main::log("Initiating IO polling for RF tone channel(s) w/ polling rate [$self->{settings}->{rfalert}->{Poll_Interval}] seconds", E_DEBUG) if $self->{'debug'};

	for my $_slot ( keys %{ $self->{'RF_Channel'} } )
	{
		&main::log("IPC Command: " . $self->{'settings'}->{'rfalert'}->{'RF_SlotDO_BinPath'} . " " . $_slot . " " . 1, E_DEBUG) if $self->{'debug'};

		my $out = IPC::System::Simple::capture(EXIT_ANY, $self->{'settings'}->{'rfalert'}->{'RF_SlotDO_BinPath'}, $_slot, 1);

		unless ( $EXITVAL == 0 )
		{
			$self->{'IO_Error'} = {
				'RetVal'	=> $EXITVAL,
				'Out'		=> $out
			};

			return -1;
		}
		elsif ( $out != 0 )
		{
			my @io_stat = &main::hex2bin($out, 1);
			my $_i = 0;

			foreach ( @io_stat )
			{
				&main::log("Checking active wait interval for active IO channel [$_i]: Active Wait Timestamp => [$self->{ActiveWait}->{ $self->{RF_Channel}->{ $_slot }->{ $_i }->{ToneId} } Current Timestamp: [" . Time::HiRes::gettimeofday . "] Interval => [ " . floor( Time::HiRes::tv_interval ([$self->{'ActiveWait'}->{ $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'ToneId'} }], [Time::HiRes::gettimeofday]) * 1000 ) . "] Active Wait Setting => [$self->{settings}->{rfalert}->{ActiveWait}]") if $self->{'debug'};

				if ( $_ > 0 && floor( Time::HiRes::tv_interval ([$self->{'ActiveWait'}->{ $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'ToneId'} }], [Time::HiRes::gettimeofday]) * 1000 ) > $self->{'settings'}->{'rfalert'}->{'ActiveWait'} )
				{
					&main::log("RF tone activation received on IO input $_i (Tone ID #" . $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'ToneId'} . "), preparing RF incident alerting");

					$self->{'led_display'} = undef;

					$self->{'ActiveWait'}->{ $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'ToneId'} } = Time::HiRes::gettimeofday;

					&main::log("Setting timestamp for active wait [$self->{ActiveWait}->{ $self->{RF_Channel}->{ $_slot }->{ $_i }->{ToneId} }]");

					$self->{'led_display'} = "<" .
					( $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'DisplayColor'} ?
						$self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'DisplayColor'} : 'green'
					) . " />$self->{RF_Channel}->{ $_slot }->{ $_i }->{DisplayLabel} " if $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'DisplayLabel'};

					&main::log("Writing RF tone activation assignment(s) to database");

					eval {
						$self->{'dbh'}->run( sub {
							$_->do(
								qq{
									INSERT INTO RF_Incident
									VALUES ( NULL, NULL, ? )
								},
								undef,
								$self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'ToneId'}
							);
						} )
					};

					if ( my $ex = $@ )
					{
						&main::log("Database exception received when saving RF tone activation address to FR incident table - Unable to assign RF tone ID to incident [$self->{RF_Channel}->{ $_slot }->{ $_i }->{ToneId}] " . $ex->error, E_ERROR);
					}

					return {
						'ToneId'			=> $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'ToneId'},
						'AudioPreamble'		=> $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'AudioPreamble'},
						'AudioAnnouncement'	=> $self->{'RF_Channel'}->{ $_slot }->{ $_i }->{'AudioAnnouncement'},
						'LED_Display'		=> $self->{'led_display'}
					};
				}

				$_i++;
			}
		}
	}
}

1;