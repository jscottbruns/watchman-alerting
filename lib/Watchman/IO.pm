package Watchman::IO;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use IO::Socket qw(:DEFAULT :crlf);
use Modbus::Client;
use FileHandle;
use Socket;
use IPC::System::Simple qw( capture $EXITVAL EXIT_ANY);
use Net::RabbitMQ;

sub new
{
    my $this = shift;
    my $params = shift;

	&main::log("Initiating Watchman::IO Device Controller");

    my $class = ref($this) || $this;
    my $self = {
    	'path'				=> $::PATH,
    	'debug'				=> $::DEBUG,
    	'temp_dir'			=> $::TEMP_DIR,
    	'dbh'				=> $::DBH,
    	'main'				=> $::self,
    	'log_dir'			=> $::LOG_DIR,
    	'units'				=> {},
    	'tones'				=> {},
    	'trigger_default'	=> 0,
    	'trigger_unit'		=> 0,
    	'trigger_tone'		=> 0,
    	'triggers'			=> [],
    	'IO_Error'			=> {}
    };

	$self->{'settings'} = {
		'system'	=> $::self->{'system'},
		'io'		=> $::self->{'io'}
	};

    $self->{'units'}->{ $_ } = 1 for @{ $params->{'Units'} };
    $self->{'tones'}->{ $_ } = 1 for @{ $params->{'ToneID'} };



	&main::log("Loading device triggers");

	my $sth;
	eval {
		$sth = $self->{'dbh'}->run( sub {
			my $sth = $_->prepare( qq{
				SELECT t1.*
				FROM IO_Device t1
			} );
			$sth->execute;
			$sth;
		} )
	};

	if ( my $ex = $@ )
	{
		&main::log("Database exception received while loading IO_Device rules - Unable to proceed with device triggers " . $ex->error, E_ERROR);
		return undef;
	}

	while ( my $rowref = $sth->fetchrow_hashref )
	{
		$self->{'trigger_unit'}++ if ( $rowref->{'EventType'} == 1 );
		$self->{'trigger_default'}++ if ( $rowref->{'EventType'} == 0 );
		$self->{'trigger_tone'}++ if ( $rowref->{'EventType'} == 2 );

		push @{ $self->{'triggers'} }, $rowref;

		# TODO: Sort triggers by type and make unique by slot/channel to prevent redundant I/O calls #
		$self->{'_triggers_'}->{ $rowref->{'TriggerType'} } = [] unless $self->{'_triggers_'}->{ $rowref->{'TriggerType'} };
		push @{ $self->{'_triggers_'}->{ $rowref->{'TriggerType'} } }, $rowref;
	}

	&main::log("Finished loading device triggers - Default triggers: [$self->{'trigger_default'}] Unit triggers: [$self->{'trigger_unit'}] Tone triggers: [$self->{'trigger_tone'}]");

	bless $self, $class;

	$self->activate_IO;
}

sub activate_IO
{
	my $self = shift;

	my $tcp_clients = {};

	for my $_trigg ( @{ $self->{'triggers'} } )
	{
		if ( $_trigg->{'EventType'} == 0 || ( $_trigg->{'EventType'} == 1 && $self->{'units'}->{ $_trigg->{'UnitID'} } == 1 ) || ( $_trigg->{'EventType'} == 2 && $self->{'tones'}->{ $_trigg->{'ToneID'} } == 1 ) )
		{
			&main::log("Activating unit IO device trigger [$_trigg->{TriggerID}] for UnitID => [$_trigg->{UnitID}]") if $_trigg->{'EventType'} == 1;
			&main::log("Activating RF tone IO device trigger [$_trigg->{TriggerID}] for ToneID => [$_trigg->{ToneID}]") if $_trigg->{'EventType'} == 2;
			&main::log("Activating default IO device trigger [$_trigg->{TriggerID}]") if $_trigg->{'EventType'} == 0;

			select(undef, undef, undef, 0.5) if $tcp_clients->{ $_trigg->{'DeviceAddr'} } && $tcp_clients->{ $_trigg->{'DeviceAddr'} } eq $_trigg->{'DevicePort'};

			$tcp_clients->{ $_trigg->{'DeviceAddr'} } = $_trigg->{'DevicePort'} if ( $_trigg->{'TriggerType'} eq 'MB_TCP' || $_trigg->{'TriggerType'} eq 'MB_RTUIP' || $_trigg->{'TriggerType'} eq 'TCPIP' );

			if ( ! ( fork ) )
			{
				&IO_ModbusTcp( $_trigg ) if $_trigg->{'TriggerType'} eq 'MB_TCP';
				&IO_ModbusRtu( $_trigg ) if $_trigg->{'TriggerType'} eq 'MB_RTU';
				&IO_ModbusRtuIp( $_trigg ) if $_trigg->{'TriggerType'} eq 'MB_RTUIP';
				&IO_TcpIp( $_trigg ) if $_trigg->{'TriggerType'} eq 'TCPIP';
				# &IO_LinPAC( $_trigg, $self->{'settings'}->{'io'}->{'IO_SlotDO_BinPath'} ) if $_trigg->{'TriggerType'} eq 'SLOT_IO';
				&IO_LinPAC_amqp( $_trigg ) if $_trigg->{'TriggerType'} eq 'SLOT_IO';
				# TODO: Add support for analog input device output control

				exit 0;
			}
		}
	}
}

sub IO_TcpIp
{
	my $trigg = shift;

	&main::log("Initiating device trigger for TCP/IP event [$trigg->{TriggerID}]");

	$/ = CRLF;

	my $sock;

	&main::log("Trigger [$trigg->{TriggerID}]: Opening socket to remote host $trigg->{DeviceAddr}:$trigg->{DevicePort} for device activation");

	eval {
		$sock = new IO::Socket::INET(
	    	PeerAddr => $trigg->{'DeviceAddr'},
		    PeerPort => $trigg->{'DevicePort'},
		    Proto    => 'tcp',
		    Timeout	 => 5
		)
	};

	if ( $@ )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Error opening IO socket to remote host $trigg->{DeviceAddr}:$trigg->{DevicePort} $@", E_ERROR);
		return undef;
	}

	if ( $sock )
	{
		$sock->autoflush(1);

		&main::log("Trigger [$trigg->{TriggerID}]: Writing --ON-- command [$trigg->{DeviceOnVal}] to remote device");

		print $sock $trigg->{'DeviceOnVal'} . CR;
		sleep 1;

		chomp( my $res = <$sock> );
		&main::log("Trigger [$trigg->{TriggerID}]: Remote device response to --ON-- command: [$res]");

		$sock->close();
	}

	if ( $trigg->{'ResetTime'} && $trigg->{'ResetTime'} > 0 )
	{
		my $current_iolock;
		my $iolock = &iolock( $trigg->{'TriggerID'} );

		&main::log("Trigger [$trigg->{TriggerID}]: Writing IO lockfile [$iolock]");

		&main::log("Trigger [$trigg->{TriggerID}]: Pausing for [$trigg->{ResetTime}] seconds for device reset");

		sleep $trigg->{'ResetTime'};

		&main::log("Trigger [$trigg->{TriggerID}]: Checking for valid IO lock [$iolock]");

		if ( open(FHIO, "<" . File::Spec->catfile( $::PATH, 'io_' . $trigg->{'TriggerID'} . '.lock' ) ) )
		{
			$current_iolock = <FHIO>;
			close FHIO;
		}

		if ( $current_iolock && $current_iolock == $iolock )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: IO lock is valid, Opening socket to remote host $trigg->{DeviceAddr}:$trigg->{DevicePort} for device reset");

			eval {
				$sock = new IO::Socket::INET(
			    	PeerAddr => $trigg->{'DeviceAddr'},
				    PeerPort => $trigg->{'DevicePort'},
				    Proto    => 'tcp',
				    Timeout	 => 5
				)
			};

			if ( $@ )
			{
				&main::log("Trigger [$trigg->{TriggerID}]: Error opening IO socket to remote host $trigg->{DeviceAddr}:$trigg->{DevicePort} $@", E_ERROR);
				return undef;
			}

			if ( $sock )
			{
				$sock->autoflush(1);

				&main::log("Trigger [$trigg->{TriggerID}]: Writing --OFF-- command [$trigg->{DeviceOffVal}] to remote device");

				print $sock $trigg->{'DeviceOffVal'} . CR;
				sleep(1);

				chomp( my $res = <$sock> );
				&main::log("Trigger [$trigg->{TriggerID}]: Remote device response to --OFF-- command: [$res]");

				$sock->close();
			}
		}
		else
		{
			&main::log("Trigger [$trigg->{TriggerID}]: IO lock [$current_iolock] is expired - Skipping IO reset");
		}
	}

	&main::log("Finished processing device trigger for TCP/IP event [$trigg->{TriggerID}]");

	return 1;
}

sub IO_ModbusTcp
{
	my $trigg = shift;

	&main::log("Initiating device trigger for Modbus/TCP event [$trigg->{TriggerID}]");

}

sub IO_ModbusRtu
{
	my $trigg = shift;

	&main::log("Initiating device trigger for Modbus/RTU event [$trigg->{TriggerID}]");

	my ($bus, %enable_ch, %reset_ch);

	if ( $trigg->{'DeviceChan'} =~ /^([0-9,]*?)(?:\|([0-9,]*))?$/ )
	{
		$enable_ch{ $_ } = $trigg->{'DeviceOnVal'} foreach ( split ',', $1 );
		$reset_ch{ $_ } = $trigg->{'DeviceOffVal'} foreach ( split ',', $2 );
	}

	unless ( $bus = new Modbus::Client $trigg->{'DeviceAddr'} )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Error initiating Modbus::Client on serial port [$trigg->{DeviceAddr}] $@ $!", E_ERROR);
		return undef;
	}

	unless ( $unit = $bus->device( $trigg->{'DeviceID'} ) )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Error setting Modbus device [$trigg->{DeviceID}]: $@ $!", E_ERROR);
		return undef;
	}

	if ( %enable_ch )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Activating [" . ( keys %enable_ch ) . "] channels [" . ( join ',', keys %enable_ch ) . "] on remote device (ON=$trigg->{DeviceOnVal})");

		unless ( $unit->write( %enable_ch ) )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Error writing activate command to remote device $@ $!", E_ERROR);
		}
	}

	if ( %reset_ch && $trigg->{'ResetTime'} && $trigg->{'ResetTime'} > 0 )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Pausing for [$trigg->{ResetTime}] seconds for device reset");

		sleep $trigg->{'ResetTime'};

		&main::log("Trigger [$trigg->{TriggerID}]: Resetting [" . ( keys %reset_ch ) . "] channels [" . ( join ',', keys %reset_ch ) . "] on remote device (ON=$trigg->{DeviceOffVal})");

		unless ( $unit->write( %reset_ch ) )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Error writing reset command to remote device", E_ERROR);
		}
	}

	return 1;
}

sub IO_ModbusRtuIp
{
	my $trigg = shift;

	&main::log("Initiating device trigger for Modbus/RTUIP event [$trigg->{TriggerID}]");

	my ($remote, $fd, %enable_ch, %reset_ch);

	if ( $trigg->{'DeviceChan'} =~ /^([0-9,]*?)(?:\|([0-9,]*))?$/ )
	{
		$enable_ch{ $_ } = $trigg->{'DeviceOnVal'} foreach ( split ',', $1 );
		$reset_ch{ $_ } = $trigg->{'DeviceOffVal'} foreach ( split ',', $2 );
	}

	&main::log("Trigger [$trigg->{TriggerID}]: Initiating socket to remote host [$trigg->{DeviceAddr}:$trigg->{DevicePort}] for device activation");

	$fd = FileHandle->new;

	unless ( $remote = inet_aton( $trigg->{'DeviceAddr'} ) )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Error opening inet_aton socket to remote host $trigg->{DeviceAddr} $@ $!", E_ERROR);
		return undef;
	}

	unless ( socket($fd, PF_INET, SOCK_STREAM, getprotobyname('tcp') ) )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Can't create TCP socket $@ $!", E_ERROR);
		return undef;
	}

	my $paddr = sockaddr_in( $trigg->{'DevicePort'}, $remote );
	if ( connect( $fd, $paddr ) )
	{
		my ($bus, $unit);

		$fd->autoflush(1);

		unless ( $bus = Modbus::Client->new( $fd ) )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Error initiating new Modbus client: $@ $!", E_ERROR);
			return undef;
		}

		unless ( $unit = $bus->device( $trigg->{'DeviceID'} ) )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Error setting Modbus device [$trigg->{DeviceID}]: $@ $!", E_ERROR);
			return undef;
		}

		if ( %enable_ch )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Activating [" . ( keys %enable_ch ) . "] channels [" . ( join ',', keys %enable_ch ) . "] on remote device (ON=$trigg->{DeviceOnVal})");

			eval {
				$unit->write( %enable_ch )
			};

			if ( $@ )
			{
				&main::log("Trigger [$trigg->{TriggerID}]: Error writing activate command to remote device $@ $!", E_ERROR);
			}
		}

		if ( %reset_ch && $trigg->{'ResetTime'} && $trigg->{'ResetTime'} > 0 )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Pausing for [$trigg->{ResetTime}] seconds for device reset");

			sleep $trigg->{'ResetTime'};

			&main::log("Trigger [$trigg->{TriggerID}]: Resetting [" . ( keys %reset_ch ) . "] channels [" . ( join ',', keys %reset_ch ) . "] on remote device (ON=$trigg->{DeviceOffVal})");

			eval {
				$unit->write( %reset_ch )
			};

			if ( $@ )
			{
				&main::log("Trigger [$trigg->{TriggerID}]: Error writing reset command to remote device $@ $!", E_ERROR);
			}
		}

		return 1;
	}
	else
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Error connecting to remote host device $trigg->{DeviceAddr}:$trigg->{DevicePort}", E_ERROR);
		return undef;
	}

	return undef;
}

sub IO_LinPAC
{
	my $trigg = shift;
	my $binpath = shift;

	&main::log("Initiating device trigger for LinPAC SlotIO event [$trigg->{TriggerID}]");

	my ($remote, $fd, @enable_ch, @reset_ch);

	if ( $trigg->{'DeviceChan'} =~ /^([0-9,]*?)(?:\|([0-9,]*))?$/ )
	{
		@enable_ch = split ',', $1;
		@reset_ch = split ',', $2;
	}

	&main::log("Trigger [$trigg->{TriggerID}]: Opening LinPAC COM port to Slot [$trigg->{DeviceAddr}] for device activation");

	if ( @enable_ch )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Activating LinPAC IO channels [" . join(', ', @enable_ch) . "]");

		foreach ( @enable_ch )
		{
			my $cmd = '#00' . $_ . $trigg->{'DeviceOnVal'};

			&main::log("Preparing IO command [" . $binpath . " " . $trigg->{'DeviceAddr'} . " 1 1 $cmd ]");

			my $out = IPC::System::Simple::capture(EXIT_ANY, $binpath, $trigg->{'DeviceAddr'}, 1, 1, $cmd);

			unless ( $EXITVAL == 0 || $EXITVAL == 1 )
			{
				$self->{'IO_Error'} = {
					'RetVal'	=> $EXITVAL,
					'Out'		=> $out
				};

				&main::log("System error received while writing to LinPAC [$cmd] output ($EXITVAL) $out", E_ERROR);
			}

			select(undef, undef, undef, 0.25);
		}

		if ( @reset_ch && $trigg->{'ResetTime'} && $trigg->{'ResetTime'} > 0 )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Pausing for [$trigg->{ResetTime}] seconds for device reset");

			sleep $trigg->{'ResetTime'};

			&main::log("Trigger [$trigg->{TriggerID}]: Resetting LinPAC IO channels [" . join(', ', @reset_ch) . "]");

			foreach ( @reset_ch )
			{
				my $cmd = '#00' . $_ . $trigg->{'DeviceOffVal'};

				&main::log("Preparing IO command [" . $binpath . " " . $trigg->{'DeviceAddr'} . " 1 1 $cmd ]");

				my $out = IPC::System::Simple::capture(EXIT_ANY, $binpath, $trigg->{'DeviceAddr'}, 1, 1, $cmd);

				unless ( $EXITVAL == 0 || $EXITVAL == 1 )
				{
					$self->{'IO_Error'} = {
						'RetVal'	=> $EXITVAL,
						'Out'		=> $out
					};

					&main::log("System error received while writing to LinPAC [$cmd] output ($EXITVAL) $out", E_ERROR);
				}

				select(undef, undef, undef, 0.65);
			}
		}

		return 1;
	}

	return undef;
}

sub IO_LinPAC_amqp
{
	my $trigg = shift;

	&main::log("Initiating AMQP device trigger for LinPAC SlotIO event [$trigg->{TriggerID}]");

	my ($remote, $fd, @enable_ch, @reset_ch);
	my $q = 'iodevice_queue';

	if ( $trigg->{'DeviceChan'} =~ /^([0-9,]*?)(?:\|([0-9,]*))?$/ )
	{
		@enable_ch = split ',', $1;
		@reset_ch = split ',', $2;
	}

	&main::log("Trigger [$trigg->{TriggerID}]: Opening connection to AMQP server");


	my $mq = Net::RabbitMQ->new();

	eval {
		$mq->connect("localhost", { user => "guest", password => "guest" })
	};

	if ( $@ )
	{
		&main::log("Unable to connect to AMQP server ($@). Unable to continue IO device updates", E_CRIT);
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

	if ( @enable_ch )
	{
		&main::log("Trigger [$trigg->{TriggerID}]: Enabling [" . scalar @enable_ch . "] device IO devices ");

		foreach ( @enable_ch )
		{
			my $slot = $trigg->{'DeviceAddr'}; # Slot No
			my $chan = $_; # Output channel
			my $val = $trigg->{'DeviceOnVal'}; # 1 or 0

			my $msg = "$slot:$chan=$val";

			&main::log("Trigger [$trigg->{TriggerID}]: Publishing AMQP device IO request: Slot [$slot] Chan [$chan] Val [$val] Request: [$msg]");

			eval {
			     $mq->publish(
			         1,
			         $q,
			         $msg,
			         {
			             exchange    => "",
			             content
			         }
			     );
			 };

			 if ( $@ )
			 {
			     &main::log("Error publishing device IO request to AMQP queue: $@", E_ERROR);
			 }

			select(undef, undef, undef, 0.25);
		}

		if ( @reset_ch && $trigg->{'ResetTime'} && $trigg->{'ResetTime'} > 0 )
		{
			&main::log("Trigger [$trigg->{TriggerID}]: Pausing for [$trigg->{ResetTime}] seconds for device IO reset");

			sleep $trigg->{'ResetTime'};

			&main::log("Trigger [$trigg->{TriggerID}]: Resetting [" . scalar @reset_ch . "] device IO devices ");

			foreach ( @reset_ch )
			{
				my $slot = $trigg->{'DeviceAddr'}; # Slot No
				my $chan = $_; # Output channel
				my $val = $trigg->{'DeviceOffVal'}; # 1 or 0

				my $msg = "$slot:$chan=$val";

				&main::log("Trigger [$trigg->{TriggerID}]: Publishing AMQP device IO request: Slot [$slot] Chan [$chan] Val [$val] Request: [$msg]");

				eval {
				     $mq->publish(
				         1,
				         $q,
				         $msg,
				         { exchange => "" },
				         { content_type	=> "text/plain" } # TODO: Fix this
					)
				 };

				 if ( $@ )
				 {
				     &main::log("Error publishing device IO request to AMQP queue: $@", E_ERROR);
				 }

				select(undef, undef, undef, 0.65);
			}
		}

		&main::log("Disconnecting from AMQP server");

		$mq->disconnect();

		return 1;
	}

	&main::log("Disconnecting from AMQP server");

	$mq->disconnect();

	return undef;
}

sub iolock
{
	my $trigg_id = shift;
    my $io_id = int( rand(50000) ) + 5000;

    if ( open(IO_LOCK, '>', File::Spec->catfile( $::PATH, 'io_' . $trigg_id . '.lock' ) ) )
    {
    	print IO_LOCK $io_id;
    	close IO_LOCK;
    }

    return $io_id;
}

sub iolock_reset
{
	my $trigg_id = shift;

    if ( open(IO_LOCK, '<', File::Spec->catfile( $::PATH, 'io_' . $trigg_id . '.lock' ) ) )
    {
    	truncate IO_LOCK, 0;
	    close IO_LOCK;
    }
}

sub iolock_read
{
	my $trigg_id = shift;

    if ( open(IO_LOCK, '<', File::Spec->catfile( $::PATH, 'io_' . $trigg_id . '.lock' ) ) )
    {
	    my $lock = <IO_LOCK>;
	    close IO_LOCK;

	    return $lock;
    }

    return undef;
}


















1;