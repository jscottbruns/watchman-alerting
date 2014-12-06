package Watchman::SMS;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use HTML::Entities;
use DBIx::Connector;
use Exception::Class::DBI;
use SOAP::Lite;

sub new
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {
    	'path'		=> $::PATH,
    	'debug'		=> $::DEBUG,
    	'temp_dir'	=> $::TEMP_DIR,
    	'dbh'		=> $::DBH,
    	'main'		=> $::self,
    	'license'	=> $::LICENSE,
    };

	$self->{'settings'} = $::self->{'sms'};

    &main::log("Starting SMS Incident notification service");

    $self->{'AlertType'}		= $params{'AlertType'} || 'assigned';
    $self->{'Units'}			= $params{'Units'};
    $self->{'MyUnits'}			= $params{'MyUnits'};
    $self->{'BoxArea'}			= $params{'BoxArea'};
    $self->{'Q_Location'} 		= $self->{'Location'} = $params{'Location'};
    $self->{'CrossSt'}			= $params{'CrossSt'};
    $self->{'CallGroup'}		= $params{'CallGroup'};
    $self->{'CallType'}			= $params{'CallType'};
    $self->{'Nature'}			= $params{'Nature'};
    $self->{'CallLabel'}		= $params{'CallLabel'} || $self->{'Nature'} || $self->{'CallType'};
    $self->{'Narrative'}		= $params{'Narrative'};
    $self->{'IncidentNo'}		= $params{'IncidentNo'};
    $self->{'CallNo'}			= $params{'CallNo'};
    $self->{'Printout'}			= $params{'Printout'};
    $self->{'GeocodeRegexAddr'} = $params{'GeocodeRegexAddr'};
    $self->{'GeocodeRegexLoc'}  = $params{'GeocodeRegexLoc'};

    if ( $self->{'AlertType'} eq 'active' )
    {
    	$self->{'IncidentNo'} = $self->{'CallNo'};
    	$self->{'Q_Location'} = $1 if ( $self->{'Q_Location'} =~ /^(.*)\s\(/ );
		&main::log("Refining geocode query location for active incident SMS notification: $self->{Q_Location}");

    	$self->{'subject'} = $params{'Subject'};
    	$self->{'message'} = $params{'Message'};
    }

    if ( $self->{'AlertType'} eq 'assigned' )
	{
		$self->{'Location'} .= " (" . join(' & ', @{ $self->{'CrossSt'} } ) . ")" if ( $self->{'CrossSt'}->[0] || $self->{'CrossSt'}->[1] );

    	$self->{'subject'} = "[$self->{BoxArea}] $self->{CallLabel}";
        $self->{'message'} = "$self->{Location} $self->{Units}";
    }

    my ($unit_flag, @final_dispatch);

    # Prevent duplicate notifications
    &main::log("Checking incident [$self->{IncidentNo}] for any prior notification events ");

    if ( $self->{'AlertType'} eq 'assigned' && $self->{'IncidentNo'} )
    {
    	my $PrevSMS;
    	eval {
			$PrevSMS = $self->{'dbh'}->run( sub {
				return $_->selectrow_hashref(
					qq{
						SELECT
							Timestamp,
							CallType,
							UnitList
						FROM SMSNotifications
						WHERE IncidentNo = ?
					},
					undef,
					$self->{'IncidentNo'}
				)
			} )
    	};

    	if ( my $ex = $@ )
    	{
    		&main::log("Database exception received during prior SMS notification lookup - Unable to fetch previous notifications " . $ex->error, E_ERROR);
    	}

	    if ( $PrevSMS )
	    {
	        &main::log("Previous notifications found for Incident $self->{IncidentNo} ($PrevSMS->{Timestamp}) ");

			$self->{'prior_flag'} = 1;
			my $prior_units;
			if ( $self->{'MyUnits'} )
			{
		        $prior_units = [ split(' ', $PrevSMS->[2]) ];

		        foreach my $_u ( @{ $self->{'MyUnits'} } )
		        {
					my $_i = &main::indexArray($_u, @{ $prior_units });
	                $unit_flag = 1 if $_i == -1;
		        }

		        if ( $unit_flag )
		        {
	            	$prior_units = [ @{ $prior_units }, @{ $self->{'MyUnits'} } ];

					my %seen = ();
	                $prior_units = [ grep { ! $seen{$_} ++ } @{ $prior_units } ];
		        }
			}

            if ( $PrevSMS->[1] ne $self->{'CallType'} || $unit_flag )
            {
            	&main::log("Incident parameters have changed, updating notifications database");

				eval {
					$self->{'dbh'}->run( sub {
						$_->do(
							qq{
								UPDATE SMSNotifications
								SET CallType = ?, UnitList = ?
								WHERE IncidentNo = ?
							},
							undef,
							$self->{'CallType'},
							join(' ', @{ $prior_units }),
							$self->{'IncidentNo'},
						);
					} )
				};

				if ( my $ex = $@ )
				{
					&main::log("Database exception received during notification table update - Unable to update previous incident notification " . $ex->error, E_ERROR);
				}
			}
            else
            {
		    	&main::log("Duplicate incident. Notifications not required");
		        return -1;
		    }
		}
		else
		{
	        &main::log("No previous notification event found for incident $self->{IncidentNo} - Adding incident to notification database");
			eval {
				$self->{'dbh'}->run( sub {
					$_->do(
						qq{
							INSERT INTO SMSNotifications
							( EventNo, IncidentNo, CallType, UnitList )
							VALUES ( ?, ?, ?, ? )
						},
						undef,
						$self->{'CallNo'},
						$self->{'IncidentNo'},
						$self->{'CallType'},
						join(' ', @{ $self->{'MyUnits'} } ),
					);
				} )
			};

			if ( my $ex = $@ )
			{
				&main::log("Database exception received during notification table update - Unable to update previous incident notification " . $ex->error, E_ERROR);
			}
	    }
    }

    $self->{'Q_Location'} .= ", $self->{main}->{system}->{RegionCounty}" if $self->{'main'}->{'system'}->{'RegionCounty'};
    $self->{'Q_Location'} .= ", $self->{main}->{system}->{RegionState}" if $self->{'main'}->{'system'}->{'RegionState'};

    &main::log("Setting geocode query location [$self->{Q_Location}]");

	bless $self, $class;

    return $self->sms_send;
}

sub sms_send
{
    my $self = shift;

    my @recip;
    my ($skip, $sleep, $awake, $smssql, $prior_recip);

	if ( $self->{'AlertType'} eq 'active' )
	{
    	$smssql = "t2.AlertQueueNotify = 1 AND t2.Inactive = 0";
	}
	else
	{
		my @context_type = qw( * );
		push @context_type, $self->{'CallType'} if $self->{'CallType'};
		push @context_type, "callgroup:$self->{CallGroup}" if $self->{'CallGroup'};

		my @context_unit = qw( * );
		push @context_unit, @{ $self->{'MyUnits'} } if $self->{'MyUnits'};

	    $smssql = <<SQL;
(
	(
		t1.AlertPref = 'CALLTYPE' AND t1.AlertContext IN ( @{[ join(', ', map { "'$_'" } @context_type) ]} )
	)
	OR
	(
		t1.AlertPref = 'UNIT' AND t1.AlertContext IN ( @{[ join(', ', map { "'$_'" } @context_unit ) ]} )
	)
)
AND t2.Inactive = 0
SQL
	}

	&main::log("Looking up SMS recipient list by call type [$self->{CallType}] and unit assignment [" . join(' ', @{ $self->{MyUnits} }) . "]");

    my $smsrecip;
    eval {
		$smsrecip = $self->{'dbh'}->run( sub {
			return $_->selectall_arrayref(
				"SELECT
					t2.SMSID,
					t2.MemberName,
					t2.PhoneNo,
					t2.Carrier,
					t2.ScheduleTimeOfDay,
					t2.ScheduleDayOfWeek
				FROM SMSAlertPrefs t1
				LEFT JOIN SMSAlerts t2 ON t2.SMSID = t1.SMSID
				WHERE $smssql
				GROUP BY t2.SMSID",
				{ Slice => {} }
			);
		} )
    };

   	if ( my $ex = $@ )
   	{
   		&main::log("Database exception received during prior SMS notification lookup - Unable to fetch previous notifications " . $ex->error, E_ERROR);
   	}

    if ( $self->{'prior_flag'} )
    {
		&main::log("Previous incident notifications have been sent for this incident, looking up previous recipients list");

		eval {
			$prior_recip = $self->{'dbh'}->run( sub {
				my ($_recip) = $_->selectrow_array(
					qq{
						SELECT SMSRecipients
						FROM SMSNotifications
						WHERE IncidentNo = ?
					},
					undef,
					$self->{'IncidentNo'}
				);
				return [ split('\|', $_recip) ];
			} )
		};
		if ( my $ex = $@ )
		{
			&main::log("Database exception received when fetching previous incident sms recipients - Unable to load previous SMS recipient list " . $ex->error, E_ERROR);
		}
    }

    if ( $smsrecip )
    {
		foreach my $_recip ( @{ $smsrecip } )
		{
            undef $sleep;
            undef $awake;
            undef $skip;

            # Checking time schedule for SMS recipient
        	if ( $_recip->{'ScheduleTimeOfDay'} =~ /([0-9]{4})-([0-9]{4})/ )
        	{
                $sleep = $2;
                $awake = $1;

                if ( $_recip->{'ScheduleDayOfWeek'} )
                {
                    $skip = 1;
                    my @day_sched = split(",", $_recip->{'ScheduleDayOfWeek'});
                    foreach ( @day_sched )
                    {
                    	if ( POSIX::strftime('%w', localtime) == $_ )
                    	{
                        	undef $skip;
                            last;
                        }
                    }
                }
        	}

            if ( $self->{'prior_flag'} && @{ $prior_recip } )
            {
				my $_i = &main::indexArray($_recip->{'PhoneNo'}, @{ $prior_recip });
                next if ( $_i > -1 );
            }

            push @recip, $_recip->{'PhoneNo'} . ( $_recip->{'Carrier'} ? "\@$_recip->{Carrier}" : undef ) if ( ! defined $skip && ( ! defined $awake || ( defined $awake && POSIX::strftime('%H%M', localtime) >= $awake ) ) && ( ! defined $sleep || ( defined $sleep && POSIX::strftime('%H%M', localtime) < $sleep ) ) );
        }
    }

	if ( ! @recip )
	{
		&main::log("SMS recipient list is empty, no incident notifications to send at this time");
		return -1;
	}

    &main::log("Preparing SMS incident notifications for " . ( $#recip + 1 ) . " recipient(s)");
    &main::log("SMS recipient list: [ " . join(', ', @recip) . " ]") if $self->{'debug'};

    eval {
    	$self->{'dbh'}->run( sub {
    		$_->do(
    			qq{
    				UPDATE SMSNotifications
    				SET SMSRecipients = ?
    				WHERE IncidentNo = ?
    			},
    			undef,
				( @{ $prior_recip } ? join(',', @{ $prior_recip }, @recip) : join(',', @recip) ),
				$self->{'IncidentNo'}
    		);
    	} )
    };

	if ( my $ex = $@ )
	{
		&main::log("Database exception received during SMSNotifications table update - Unable to update SMSNotifications table with SMS recipient list " . $ex->error, E_ERROR);
	}

    &main::log("Setting SMS subject: [$self->{subject}]");
    &main::log("Setting SMS message: [$self->{message}]");

	&main::log("Connecting to SMS incident notification relay service [$self->{settings}->{SMS_RelayProxyAddr}/$self->{settings}->{SMS_RelayURI_Addr}] ");

	my $args = {
		service				=> 'sms_send',
		license				=> $self->{'license'},
		recip				=> [ @recip ],
		subject				=> $self->{'subject'},
		message				=> $self->{'message'},
		debug				=> $self->{'debug'},
		data				=> $self->{'Printout'},
		inc_no				=> $self->{'IncidentNo'},
		location			=> $self->{'Location'},
		q_location			=> $self->{'Q_Location'},
		geocodeRegexAddr	=> $self->{'GeocodeRegexAddr'},
		geocodeRegexLoc		=> $self->{'GeocodeRegexLoc'}
	};

	my $response;
	if (
		$response = SOAP::Lite
			->uri( $self->{'settings'}->{'SMS_RelayURI_Addr'} )
			->proxy
			(
				$self->{'settings'}->{'SMS_RelayProxyAddr'},
				timeout	=> $self->{'settings'}->{'SMS_HTTP_Timeout'} || 10
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

	&main::log("Error during SMS incident notification service SOAP HTTP request initilization $@ $!", E_ERROR);
	return undef;
}

sub update_printout
{
    my $self = shift;

    &main::log("Starting incident update service");

    &main::log("Initiating SOAP client [ $self->{'settings'}->{'SMS_RelayURI_Addr'} ] \n");

    my $args = {
        'service'          => 'update_printout',
        'license'          => $self->{'license'},
        'data'             => $self->{'StreamData'},
        'inc_no'           => $self->{'IncidentNo'},
        'location'         => $self->{'Location'},
        'q_location'       => $self->{'Q_Location'},
        'geocodeRegexAddr' => $self->{'GeocodeRegexAddr'},
        'geocodeRegexLoc'  => $self->{'GeocodeRegexLoc'},
    };

    my $response;
    unless (
    	$response = SOAP::Lite
    		->uri( $self->{'settings'}->{'SMS_RelayURI_Addr'} )
    		->proxy
    		(
    			$self->{'settings'}->{'SMS_RelayProxyAddr'},
    			timeout	=> $self->{'settings'}->{'SMS_HTTP_Timeout'}
    		)
			->on_fault
			(
				sub
				{
					my ($soap, $res) = @_;
					&main::log( ref $res ? $res->faultstring : $soap->transport->status . " - SMS incident update service failed", E_ERROR);
				}
			)
    		->init( $args )
    )
    {

        &main::write_log("Server error received from SOAP server\n", 1);
        return undef;
    }

    if ( $response =~ /(-?[0-9])\n?(.*)?/m )
    {
        my $res = $1;
        my $msg = $2;

        if ( $res == 1 )
        {
            &main::write_log("Incident update service successful");
            return 1;
        }
        else
        {
            &main::write_log("Errors encountered during incident update service: $msg", E_ERROR);
            return undef;
        }
    }
    else
    {
        &main::write_log("Errors encountered during incident update service - Unable to parse server response: $response", E_ERROR);
        return undef;
    }
}

1;