package Watchman::RSS;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use HTML::Entities;
use SOAP::Lite;

sub new {
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {
    	path		=> $::PATH,
    	debug		=> $::DEBUG,
    	temp_dir	=> $::TEMP_DIR,
    	dbh			=> $::DBH,
    	main		=> $::self,
    	license		=> $::LICENSE,
    };

	$self->{'settings'} = $self->{'main'}->{'rss'};

    &main::write_log("Starting Watchman::RSS - Incident feed update service");

    $self->{'Units'}		= $params{'Units'};
    $self->{'MyUnits'}		= $params{'MyUnits'};
    $self->{'BoxArea'}		= $params{'BoxArea'};
    $self->{'Location'}		= $params{'Location'};
    $self->{'CallGroup'}	= $params{'CallGroup'};
    $self->{'CallType'}		= $params{'CallType'};
    $self->{'CallLabel'}	= $params{'CallLabel'} || $self->{'Nature'} || $self->{'CallType'};
    $self->{'Narrative'}	= $params{'Narrative'};
    $self->{'IncidentNo'}	= $params{'IncidentNo'};
    $self->{'CallNo'}		= $params{'CallNo'};

    my $unit_flag;

    &main::write_log("Checking for previous RSS events for Incident $self->{IncidentNo}");

   	eval {
		($PrevTS, $PrevCallType, $PrevUnits) = $self->{'dbh'}->run( sub {
			return $_->selectrow_arrayref(
				qq{
					SELECT
						Timestamp,
						CallType,
						UnitList
					FROM RSSFeed
					WHERE IncidentNo = ?
				},
				undef,
				$self->{'IncidentNo'}
			)
		} )
    };

    if ( my $ex = $@ )
    {
    	&main::log("Database exception received during prior RSS update lookup - Unable to fetch previous RSS updates " . $ex->error, E_ERROR);
    }

	if ( $PrevTS && $PrevCallType )
	{
		&main::log("Previous RSS updates found for Incident $self->{IncidentNo} ($PrevTS) ");

		$self->{'prior_flag'} = 1;
	    my $prior_units = [ split(' ', $PrevUnits) ];

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

        if ( $PrevCallType ne $self->{'CallType'} || $unit_flag )
        {
        	&main::log("Incident parameters have changed, updating RSS feed database");

			eval {
				$self->{'dbh'}->run( sub {
					$_->do(
						qq{
							UPDATE RSSFeed
							SET CallType = ?, Nature = ?, UnitList = ?
							WHERE IncidentNo = ?
						},
						undef,
						$self->{'CallType'},
						$self->{'Nature'},
						join(' ', @{ $prior_units }),
						$self->{'IncidentNo'},
					);
				} )
			};

			if ( my $ex = $@ )
			{
				&main::log("Database exception received during RSS table update - Unable to update previous incident RSS feed  " . $ex->error, E_ERROR);
			}
		}
        else
        {
			&main::log("Duplicate incident. RSS updates not required");
		    return -1;
		}
	}
	else
	{
        &main::log("No previous RSS events found for incident $self->{IncidentNo} - Adding incident to RSS database");
		eval {
			$self->{'dbh'}->run( sub {
				$_->do(
					qq{
						INSERT INTO RSSFeed
						( IncidentNo, CallType, Nature, UnitList )
						VALUES ( ?, ?, ?, ? )
					},
					undef,
					$self->{'IncidentNo'},
					$self->{'CallType'},
					$self->{'Nature'},
					join(' ', @{ $self->{'MyUnits'} } )
				);
			} )
		};

		if ( my $ex = $@ )
		{
			&main::log("Database exception received during notification table update - Unable to update previous incident notification " . $ex->error, E_ERROR);
		}
    }

    bless $self, $class;
    bless $self->{'dbh'}, 'DBI::db';

    return $self->rss_update;
}

sub rss_update
{
    my $params = {
        'service'   => 'rss_feed',
        'license'   => $self->{'license'},
        'inc_no'    => $self->{'IncidentNo'},
        'area'      => $self->{'BoxArea'},
        'units'     => $self->{'Units'},
        'location'  => $self->{'Location'},
        'callGroup' => $self->{'CallGroup'},
        'callType'  => $self->{'CallType'},
        'typeName'  => $self->{'CallLabel'},
        'text'      => $self->{'Narrative'}
    };

    &main::write_log("Connecting to RSS relay server");

    my $response;
    unless (
    	$response = SOAP::Lite
    		->uri( $self->{'settings'}->{'RSS_RelayURI_Addr'} )
    		->proxy
    		(
    			$self->{'settings'}->{'RSS_RelayProxyAddr'},
    			timeout	=> $self->{'RSS_HTTP_Timeout'}
    		)
			->on_fault
			(
				sub
				{
					my ($soap, $res) = @_;
					&main::log( ref $res ? $res->faultstring : $soap->transport->status . " - RSS incident feed service failed", E_ERROR);
				}
			)
    		->init( $params )
	)
	{
    	&main::write_log("RSS feed service encountered errors - HTTP SOAP server error $@ $! ", E_ERROR);
        return undef;
    }

    if ( $response =~ /(-?[0-9])\n?(.*)?/m )
    {
        my $res = $1;
        my $msg = $2;

        if ( $res == 1 )
        {
            &main::write_log("RSS incident feed service successful");
            return 1;
        }
        else
        {
            &main::write_log("Errors encountered during RSS feed update: $msg", E_ERROR);
            return undef;
        }
    }
    else
    {
        &main::write_log("Errors encountered during RSS feed update - Unable to parse server response: $response", E_ERROR);
        return undef;
    }
}
1;