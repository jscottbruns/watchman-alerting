package Watchman::AlertQueue;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

# **Note - Modification required for HTTP keep alive. Changed LWP::Protocol::http#206. Removed header "Keep-Alive: 300"

use LWP::UserAgent;
use HTTP::Headers;
use HTTP::Message;
use HTTP::Cookies;
use HTML::Entities;
use JSON;
use File::Temp qw( tempfile );
use Data::Dumper;

my ($host_url, $cookie_name, $valid_session);

sub new
{
    my $this = shift;

	&main::log("Starting Watchman::AlertQueue Active Incident Polling System");

    my $class = ref($this) || $this;
    my $self = {
    	'path'				=> $::PATH,
    	'debug'				=> $::DEBUG,
    	'temp_dir'			=> $::TEMP_DIR,
    	'debug_dir'			=> $::DEBUG_DIR,
    	'dbh'				=> $::DBH,
    	'main'				=> $::self,
    	'log_dir'			=> $::LOG_DIR,
    	'cookie_jar'		=> undef,
    	'ua'				=> undef,
    	'alert_msg'			=> undef,
    	'call_alert'		=> {},
    	'alert_controls'	=> {},
    	'AlertQueueAlert'	=> {},
    	'BoxAreaTTS'		=> {}, 
    	'req_err'			=> 0,
    	'station'			=> $::self->{'system'}->{'DefaultStationID'},
    	'box_substr_start'	=> undef,
    	'box_substr_end'	=> undef,
    	'CallGroup'			=> {},
    	'FirstDueRegex'		=> '',
    	'LED_DisplayMode'	=> $::self->{'ledalerting'}->{'LED_StandardDisplayMode'},
    	'LED_TextAddr'		=> $::self->{'ledalerting'}->{'LED_DefaultTextPage'}
    };

	$self->{'settings'} = $::self->{'alertqueue'};

	$self->{'LED_DisplayMode'} = 'rotate' unless $self->{'LED_DisplayMode'};
	$self->{'LED_TextAddr'} = 'A' unless $self->{'LED_TextAddr'}; 
	$self->{'settings'}->{'PollingLookbackTime'} = $self->{'settings'}->{'PollingLookbackTime'} || 300;
	$self->{'settings'}->{'HTTP_CookieSaveFile'} = $self->{'settings'}->{'HTTP_CookieSaveFile'} || File::Spec->catfile( $self->{'path'}, "http_cookies.lwp");  
	$self->{'settings'}->{'ActiveIncAudioPreamble_FirstDue'} = $self->{'settings'}->{'ActiveIncAudioPreamble'} unless $self->{'settings'}->{'ActiveIncAudioPreamble_FirstDue'};

	if ( $self->{'settings'}->{'BoxAreaSubstr'} =~ /^([0-9]{1,})?\|([0-9]{1,})?$/ )
	{
		$self->{'box_substr_start'} = $1 if defined $1;
		$self->{'box_substr_end'} = $2 if defined $2;
	}
		
	$self->{'box_substr_start'} = 0 unless defined $self->{'box_substr_start'};
	$self->{'FirstDueRegex'} = qr/$self->{settings}->{FirstDueBoxListing}/;

    &main::log("Loading AlertQueue filters and contexts");

	my $sth;
	eval {
		$sth = $self->{'dbh'}->run( sub {
			my $sth = $_->prepare( qq{
				SELECT
					t1.Agency,
					t1.BoxArea,
					t1.CallType,
					t1.AreaAnnounce AS BoxAreaAnnounce
				FROM AlertQueueFilter t1
			} );
			$sth->execute;
			$sth;
		} )
	};

	if ( my $ex = $@ )
	{
		&main::log("Database exception received during AlertQueue context lookup - Unable to proceed with AlertQueue startup " . $ex->error, E_ERROR);
		return undef;
	}
	
	&main::log("Loading call groups and group settings");

	my $arrlist;
	eval {
		$arrlist = $self->{'dbh'}->run( sub {
			return $_->selectall_arrayref(
				q{
					SELECT *
					FROM CallGroup
				},
				{ Slice => {} }
			);
		} )
	};
	
	if ( my $ex = $@ )
	{
		&main::log("DBI exception during station unit lookup: $ex->error - Unable to perform unit alerting", E_ERROR);
	}

	foreach ( @{ $arrlist } )
	{
	    $self->{'CallGroup'}->{ $_->{'CallGroup'} } = $_;
	}
	
	&main::log("Loading call types and type settings");
	
	my $arrlist;
	eval {
		$arrlist = $self->{'dbh'}->run( sub {
			return $_->selectall_arrayref(
				q{
					SELECT *
					FROM CallType
				},
				{ Slice => {} }
			);
		} )
	};
	
	if ( my $ex = $@ )
	{
		&main::log("DBI exception during station unit lookup: $ex->error - Unable to perform unit alerting", E_ERROR);
	}

	foreach ( @{ $arrlist } )
	{
	    $self->{'CallType'}->{ $_->{'TypeCode'} } = $_;
	}	
							
	
	$self->{'AlertQueueTypes'} = [];
	$self->{'AlertQueueType'} = {};
	$self->{'AlertQueueGroup'} = {};	
				
	while ( my $rowref = $sth->fetchrow_hashref )
	{	
		$self->{'BoxAreaTTS'}->{ $rowref->{'BoxArea'} } = ( defined $rowref->{'BoxAreaAnnounce'} && -f File::Spec->catfile( $self->{'path'}, $rowref->{'BoxAreaAnnounce'} ) ? $rowref->{'BoxAreaAnnounce'} : undef ) unless $self->{'BoxAreaTTS'}->{ $rowref->{'BoxArea'} };
		
		foreach my $_t ( split '\|', $rowref->{'CallType'} )
		{		
			if ( $_t =~ /^(?:(type|group):)?(.*)$/ )
			{
				if ( $1 eq 'group' )
				{
					push @{ $self->{'AlertQueueTypes'} }, {
						'CallType'		=> $2,
						'Switch'		=> 'group',
						'Agency'		=> $rowref->{'Agency'},	
						'TypeAnnounce'	=> ( defined $self->{'CallGroup'}->{ $2 }->{'AlertAudioAnnounce'} && -f File::Spec->catfile( $self->{'path'}, $self->{'CallGroup'}->{ $2 }->{'AlertAudioAnnounce'} ) ? $self->{'CallGroup'}->{ $2 }->{'AlertAudioAnnounce'} : undef ),
						'DisplayColor'	=> $self->{'CallGroup'}->{ $2 }->{'AlertDisplayColor'},
						'Priority'		=> $self->{'CallGroup'}->{ $2 }->{'Priority'}
					};			
					$self->{'AlertQueueAlert'}->{ $rowref->{'BoxArea'} }->{'Groups'}->{ $2 } = $#{ $self->{'AlertQueueTypes'} };		
				}				
				elsif ( $1 eq 'type' || ! $1 )
				{					
					push @{ $self->{'AlertQueueTypes'} }, {
						'CallType'		=> $2,
						'Switch'		=> 'type',
						'Agency'		=> $rowref->{'Agency'},
						'Label'			=> undef,
						'TypeAnnounce'	=> undef,
						'DisplayColor'	=> undef 
					};
					
					if ( $type_sth->execute( $2 ) )		
					{
						my $typerow = $type_sth->fetchrow_hashref;
						
						$self->{'AlertQueueTypes'}->[ $#{ $self->{'AlertQueueTypes'} } ]->{'Label'}			= $typerow->{'CallTypeLabel'};				
						$self->{'AlertQueueTypes'}->[ $#{ $self->{'AlertQueueTypes'} } ]->{'TypeAnnounce'}	= ( defined $typerow->{'CallTypeAnnounce'} && -f File::Spec->catfile( $self->{'path'}, $typerow->{'CallTypeAnnounce'} ) ? $typerow->{'CallTypeAnnounce'} : undef );
						$self->{'AlertQueueTypes'}->[ $#{ $self->{'AlertQueueTypes'} } ]->{'DisplayColor'}	= $typerow->{'LED_DisplayColor'};
						$self->{'AlertQueueTypes'}->[ $#{ $self->{'AlertQueueTypes'} } ]->{'Priority'}		= $typerow->{'Priority'};
					}			
					$self->{'AlertQueueAlert'}->{ $rowref->{'BoxArea'} }->{'Types'}->{ $2 } = $#{ $self->{'AlertQueueTypes'} };		
				}								
			}					
		}
	}			

    bless $self, $class;

    unless ( $self->init_cookie )
    {
        &main::log("Error creating cookie object", E_ERROR);
        return undef;
    }

    unless ( $self->init_lwp )
    {
        &main::log("Error creating LWP object", E_ERROR);
        return undef;
    }

	$self->{'PollHost'} = $self->{'settings'}->{'HostAddr'} . $self->{'settings'}->{'PollingPath'};

	# Global variables used for HTTP session validation
	$valid_session = 0;
	$host_url = $self->{'settings'}->{'HostAddr'};
	$cookie_name = $self->{'settings'}->{'HTTP_CookieName'} || 'PHPSESSID';

    &main::log("Checking previous HTTP cookies for existing valid [ $cookie_name ] sessions on host [ $host_url ]");

    $self->{'cookie_jar'}->scan( \&Watchman::AlertQueue::validate_cookie );

    if ( ! $valid_session )
    {
		&main::log("No existing HTTP sessions found - Initiating AlertQueue client authentication");

        unless (
        	$self->init_session(
        		HostAddr		=> $self->{'settings'}->{'HostAddr'},
        		LoginPath		=> $self->{'settings'}->{'LoginPath'},
        		QueryString		=> $self->{'settings'}->{'LoginAddrQueryString'},
        		LoginHTTP_Post	=> $self->{'settings'}->{'LoginHTTP_Post'},
        		Username		=> $self->{'settings'}->{'LoginUsername'},
        		Password		=> $self->{'settings'}->{'LoginPassword'},
        		FailRegex		=> $self->{'settings'}->{'LoginFailureRegex'}
        	)
        ) {
            &main::log("Error received during AlertQueue client authentication", E_ERROR);
            return undef;
        }

        &main::log("AlertQueue client authentication complete");

    }

	&main::log("Valid AlertQueue client session found, authentication not required") if $valid_session;

	if ( $self->{'settings'}->{'hostsync'}->{'HostSyncEnabled'} )
	{
		&main::log("Initializing AlertQueue HostSync service");

		unless (
			$self->init_hostsync(
				DB_Driver	=> $self->{'settings'}->{'hostsync'}->{'HostSyncDB_Driver'},
				DB_Name		=> $self->{'settings'}->{'hostsync'}->{'HostSyncDB_Name'},
				DB_Port		=> $self->{'settings'}->{'hostsync'}->{'HostSyncDB_Port'},
				DB_User		=> $self->{'settings'}->{'hostsync'}->{'HostSyncDB_User'},
				DB_Pass		=> $self->{'settings'}->{'hostsync'}->{'HostSyncDB_Pass'},
			)
		) {
			undef $self->{'settings'}->{'hostsync'}->{'HostSyncEnabled'};
			&main::log("Unrecoverable errors encountered during HostSync initialization. HostSync service disabled. ", E_ERROR);
		}
	}

    return $self;
}

sub init_cookie
{
    my $self = shift;

    &main::log("Initiating HTTP cookie jar: $self->{settings}->{HTTP_CookieSaveFile} ");

	$self->{'cookie_jar'} = HTTP::Cookies->new(
	    'file'              =>  $self->{'settings'}->{'HTTP_CookieSaveFile'},
	    'ignore_discard'    =>  1,
	);

	return undef unless $self->{'cookie_jar'};
}

sub init_lwp
{
    my $self = shift;

    &main::log("Initiating LWP::UserAgent Object");

	$self->{'ua'} = LWP::UserAgent->new(
		'agent'				=> "iCAD Client/1.1 (WatchmanAlerting AlertQueue Polling Agent; Standard HTTP Agent Object)",
	    'cookie_jar'        => $self->{'cookie_jar'},
	    'timeout'           => $self->{'settings'}->{'HTTP_Timeout'} || 5,
	    'default_headers'	=> HTTP::Headers->new(
	    	'Accept-Language'	=>	'en-US,en',
	    	'Accept-Charset'	=>	'iso-8859-1, utf-8, utf-16, *',
	    	'Accept-Encoding'	=>	scalar HTTP::Message::decodable(),
	    	'Accept'			=>	'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*',
	    	'Content-Type'		=>	'application/x-www-form-urlencoded',
	    )
	);

	return undef unless $self->{'ua'};
}

sub init_session
{
	my $self = shift;
    my %params = @_;

    &main::log("Initiating AlertQueue client authentication");

	$params{'LoginHTTP_Post'} =~ s/%username/$params{Username}/ if $params{'LoginHTTP_Post'} =~ /%username/;
	$params{'LoginHTTP_Post'} =~ s/%password/$params{Password}/ if $params{'LoginHTTP_Post'} =~ /%password/;

	my $login_host = $params{'HostAddr'} . $params{'LoginPath'} . ( $params{'QueryString'} ? "?" . $params{'QueryString'} : undef );
	&main::log("Submitting authentication request to AlertQueue server [ $login_host ]");

	my $response = $self->{'ua'}->post(
		$login_host,
		Content	=> $params{'LoginHTTP_Post'}
	);

    if ( $response->status_line eq '200 OK' || $response->status_line eq '302 Found' )
	{
    	&main::log("HTTP Response (" . $response->status_line . ") " . $response->headers_as_string, E_DEBUG) if $self->{'debug'};
        &main::log($response->decoded_content, E_DEBUG) if $self->{'debug'};		

        if ( $response->decoded_content =~ /^(ERROR)\s([0-9]*)$/ )
        {
			&main::log("AlertQueue client authentication failed - iCAD server responded with error [$2]");

            &main::log("Clearing HTTP authentication cookies");
            $self->{'cookie_jar'}->clear;
            $self->{'cookie_jar'}->save;

            return undef;
        }

        &main::log("Client authentication successful, committing HTTP cookies ");
        $self->{'cookie_jar'}->save;

        return 1;

    }

	&main::log("Authentication request unsuccessful - HTTP Response (" . $response->status_line . ")", E_ERROR);
	&main::log($response->decoded_content, E_DEBUG) if $self->{'debug'};

    return undef;
}

sub poll
{
    my $self = shift;

	$self->{'PollPost'} = $self->{'settings'}->{'PollingHTTP_Post'};	

	if ( $self->{'PollPost'} =~ /%OpenTime%/ )
	{
		my $lookuptime = POSIX::strftime('%Y-%m-%d %H:%M:%S', localtime( time - $self->{'settings'}->{'PollingLookbackTime'} ) );
		$self->{'PollPost'} =~ s/%OpenTime%/$lookuptime/;
	}
	elsif ( $self->{'PollPost'} =~ /%queryinterval%/ )
	{
		my $lookbacksec = $self->{'settings'}->{'PollingLookbackTime'};
		$self->{'PollPost'} =~ s/%queryinterval%/$lookbacksec/;		
	}
	else
	{
		undef $self->{'PollPost'};
	}

	&main::log("Preparing AlertQueue polling request to host [$self->{PollHost}] => [$self->{PollPost}]", E_DEBUG) if $self->{'settings'}->{'AlertQueueDebug'};
	
	my $response = $self->{'ua'}->post(
		$self->{'PollHost'},
		Content	=> $self->{'PollPost'}
	);	

	&main::log("AlertQueue HTTP response (" . $response->status_line . ") content type [" . $response->header('Content-Type') . "]", E_DEBUG) if $self->{'settings'}->{'AlertQueueDebug'};

    if ( $response->status_line =~ /$self->{settings}->{HTTP_AcceptStatus}/i )
    {
    	my ($_vars, @hostsync_inc, @inc, $substr_start, $substr_end);

	    $self->{'req_err'} = 0; # Reset the err counter
		my $content = $response->decoded_content;

		if ( $self->{'settings'}->{'AlertQueueDebug'} )
		{
			my ($tmpfh, $tmpfname) = File::Temp::tempfile(
				'AlertQueueHTTP_XXXXXX',
				DIR		=> $self->{'debug_dir'},
				SUFFIX	=> '.txt'
			);

			&main::log("Writing AlertQueue HTTP response to debug file => [$tmpfname]", E_DEBUG);
			if ( $tmpfh )
			{
				print $tmpfh $tmpfh;
				close $tmpfh;
			}						
		}

		if ( $response->header('Content-Type') =~ /application\/json/ )
		{
			unless ( $content )
			{
				&main::log("HTTP response does not contain content for JSON parsing, aborting AlertQueue polling cycle", E_ERROR);
				return undef; 	
			}
			
			my $json = JSON->new->utf8;					
			my $json_hash;
			
			eval {
				$json_hash = $json->decode( $content )
			};
			
			if ( $@ )
			{
				&main::log("JSON error parsing HTTP response $@ $! - Unable to proceed with AlertQueue polling cycle", E_ERROR);
				return undef;	
			}

			unless ( $json_hash )
			{
				&main::log("JSON parsing failed to produce valid hash array, aborting AlertQueue polling cycle", E_ERROR);
				return undef; 					
			}

			if ( $self->{'settings'}->{'AlertQueueDebug'} )
			{	
				&main::log("Writing AlertQueue JSON response to debug file => [/tmp/AlertQueue_JSON.txt]", E_DEBUG);
				
				if ( open(TMPFH, ">/tmp/AlertQueue_JSON.txt") )
				{
					print TMPFH $content;
					close TMPFH;
				}						
			}				

			$_vars = {};
			
			&main::log("AlertQueue polling request returned [$json_hash->{Total}] results", E_DEBUG) if $self->{'settings'}->{'AlertQueueDebug'};		
			
			if ( $json_hash->{'Total'} > 0 )
			{
				my $inc_date = $json_hash->{'IncidentDate'};

				my $STH = $DBH->run( sub {
					return $_->prepare( qq{
						SELECT COUNT(*) AS AlertFlag
						FROM AlertQueueRules t1
						WHERE 
						( t1.Agency IS NULL OR t1.Agency = ? )
						AND
						? REGEXP t1.BoxArea 
						AND 
						( 
							CONCAT('type:', ?) REGEXP t1.Ruleset OR CONCAT('group:', ?) REGEXP t1.Ruleset
						) 
					} );
				} ) unless $STH;				 
					
				foreach my $node ( @{ $json_hash->{'IncidentListing'} } )
				{	                
	                &main::log("Checking active incidents: INCNO [$_vars->{IncidentNo}] STAT [$_vars->{Status}] OPEN [$_vars->{OpenTime}] CLOSE [$_vars->{CloseTime}] TYPE [$_vars->{CallType}] GROUP [$_vars->{CallGroup}] NATURE [$_vars->{Nature}] BOX [$_vars->{BoxArea}] LOC [$_vars->{Location}]") if $self->{'settings'}->{'AlertQueueDebug'};

					$substr_end = $self->{'box_substr_end'};					
					$substr_end = length $_vars->{'BoxArea'} unless defined $substr_end;	

					unless ( $STH->execute( $node->{'Agency'}, $node->{'Box'}, $node->{'CallType'}, $node->{'CallGroup'} ) )
					{	
						&main::log("Error executing call data against AlertQueue ruleset $DBH->errstr", E_CRIT);
						return undef;
					}

		            if ( $STH->fetchrow_array || $node->{'CallNo'} eq $self->{'call_alert'}->{'EventNo'} || ( $node->{'Box'} =~ m/^$self->{FirstDueRegex}$/ && $node->{'CallType'} !~ /$self->{settings}->{FirstDueIgnoreTypes}/ ) )
		            {
	                    &main::log("** Possible active incident --> INCNO [CallNo->{CallNo}] STAT [$_vars->{Status}] TYPE [$_vars->{CallType}] GROUP [$_vars->{CallGroup}] NATURE [$_vars->{Nature}] BOX [$_vars->{BoxArea}] LOC [$_vars->{Location}]") if $self->{'settings'}->{'AlertQueueDebug'};

				        push @inc,
				        {
				            EventNo		=> $node->{'CallNo'},
				            IncidentNo	=> $node->{'IncidentNo'},
	                        OpenTime	=> $node->{'EntryTime'},
	                        DispTime	=> $node->{'DispatchTime'},
	                        CloseTime	=> $node->{'CloseTime'},
				            CallType	=> &main::trim( $node->{'CallType'} ),
				            CallGroup	=> $node->{'CallGroup'},
				            Nature		=> &main::trim( $node->{'Nature'} ),
				            BoxArea		=> &main::trim( $node->{'Box'} ),
				            Status		=> $node->{'Status'},
				            Location	=> &main::trim( $node->{'Location'} ),
				            Agency		=> $node->{'Agency'},
				            CityCode	=> $node->{'CityCode'}
				        };
		            }
	            }
			}
		}		

        undef $content;

	    foreach my $_i ( reverse @inc )
	    {            			
	        if ( $_i->{'Status'} != 1 ) # Pending incident or incident is already in alert queue
	        {					        	
				if ( $self->{'settings'}->{'AlertQueueFormatting_XSL'} && -f File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'AlertQueueFormatting_XSL'} ) )
				{
					&main::log("Applying XSLT formatting against stylesheet [$self->{settings}->{AlertQueueFormatting_XSL}]") if $self->{'settings'}->{'AlertQueueDebug'};
					
					if ( my $_tempfile = File::Temp::tempnam( '/tmp', 'alertqueueXXXX' ) )
					{
						my $__xsl = File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'AlertQueueFormatting_XSL'} );
						
						if ( open(XSLT, "| /usr/bin/xsltproc $__xsl - > $_tempfile") )
						{
							print XSLT "<AlertQueue><Agency>" . encode_entities( $_i->{'Agency'} ) . "</Agency><BoxArea>" . encode_entities( $_i->{'BoxArea'} ) . "</BoxArea><Location>" . encode_entities( $_i->{'Location'} ) . "</Location><CityCode>" . encode_entities( $_i->{'CityCode'} ) . "</CityCode></AlertQueue>";
							close XSLT;																														
							
							{
								local $/= undef;
								
								if ( open XSLOUT, $_tempfile )
								{
									my $__xml = <XSLOUT>;
									close XSLOUT;
									
									if ( $__xml )
									{
										&main::log("XSLT processing complete, parsing output [$__xml]") if $self->{'settings'}->{'AlertQueueDebug'};
										
										eval 
										{
											my $xp = XML::LibXML->load_xml( string => $__xml );
											
											&main::log("Applying formatted incident values") if $self->{'settings'}->{'AlertQueueDebug'};
											
											$_i->{'BoxArea'} = $xp->findnodes('//AlertQueue/BoxArea/Formatted')->to_literal if $xp->findnodes('//AlertQueue/BoxArea/Formatted')->to_literal;
											$_i->{'Box'} = $xp->findnodes('//AlertQueue/BoxArea/UnFormatted')->to_literal if $xp->findnodes('//AlertQueue/BoxArea/UnFormatted')->to_literal;
											$_i->{'FullBox'} = $xp->findnodes('//AlertQueue/BoxArea/FullBox')->to_literal if $xp->findnodes('//AlertQueue/BoxArea/FullBox')->to_literal;
											$_i->{'Location'} = $xp->findnodes('//AlertQueue/Location')->to_literal if $xp->findnodes('//AlertQueue/Location')->to_literal;											
										};
										
										if ( $@ )
										{
											&main::log("XML parsing error $@ $!", E_ERROR);
										}
									}
								}
								
								if ( $? )
								{
									&main::log("Error reading XSLT output from file [$_tempfile] ($?) $@ $!", E_ERROR);
								}
							}
						}
						
						if ( $? )
						{
							&main::log("XSLT pipe error during AlertQueue pre-processing ($?) $@ $!", E_ERROR);	
						}
					}
				}
				
				$_i->{'FullBox'} = $_i->{'BoxArea'} unless $_i->{'FullBox'};
				
				#
				# Pending Incident Cancellation - EventNo eq Active Incident EventNo, Status eq 0 (cancelled), DispatchTime eq 0 or unset
				#
                if ( $_i->{'EventNo'} eq $self->{'call_alert'}->{'EventNo'} && $_i->{'Status'} == 0 && ! $_i->{'DispTime'} ) 
                {
                    &main::log("[$self->{call_alert}->{EventNo}] ** Pending Incident Cancellation ** [$self->{call_alert}->{BoxArea}] $self->{call_alert}->{CallType} $self->{call_alert}->{Location}");

					eval {
                        $self->{'dbh'}->run( sub {
                        	$_->do(
                        		qq{
                        			UPDATE ActiveIncident
                                    SET
                                        CloseTime = ?,
                                        Cancel = 1
                                    WHERE EventNo = ?
                        		},
                        		undef,
                        		$_i->{'CloseTime'},
                        		$_i->{'EventNo'}
                        	)
                        } )
					};

					if ( my $ex = $@ )
					{
                        &main::log("Database exception received during active incident update - Unable to set incident cancel flag " . $ex->error, E_ERROR);
                    }

                    $self->{'serial_message'} = "<amber/>** CANCELLED ** ";
					
                    if ( $self->{'alert_controls'}->{'inc_type'} eq 'pending' )
                    {
                    	$self->{'serial_message'} .=
                        ( $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/ || $_i->{'FullBox'} =~ m/^$self->{FirstDueRegex}$/ ?
                            "<rainbow1/>PENDING FIRST DUE INCIDENT: " : "<green/>PENDING INCIDENT: "
                        );
                    }
                    else
                    {
                    	$self->{'serial_message'} .= "<$self->{call_alert}->{DisplayColor} />PENDING LOCAL INCIDENT: ";
                    }

                    $self->{'serial_message'} .= "[$self->{call_alert}->{BoxArea}] " .
                    ( $self->{'AlertQueueType'}->{ $self->{'call_alert'}->{'CallType'} }->{'Label'} ?
                        $self->{'AlertQueueType'}->{ $self->{'call_alert'}->{'CallType'} }->{'Label'} : $self->{'call_alert'}->{'Label'}
                    ) . " " . encode_entities( $self->{'call_alert'}->{'Location'} ) . " <amber/>** CANCELLED ** ";

                    &main::log("Setting serial message: $self->{serial_message}", E_DEBUG) if $self->{'debug'};

                    $self->{'serial_message'} = "<signboard><text label=\"$self->{LED_TextAddr}\"><mode display=\"" . $self->{'LED_DisplayMode'} . "\"/>$self->{serial_message} </text></signboard>";

                    $self->{'call_alert'} = {
                    	PreambleTone	=> $self->{'settings'}->{'ActiveIncUpdateAudioPreamble'},
                    };
                    $self->{'alert_controls'}->{'alert_type'} = 'cancel';

                    return 1;
                }
                #
                # Pending Incident Update
                # EventNo eq Active Incident EventNo, Status eq -1 (pending), Active Incident Values for Box, Type and/or Location ne to Active Incident Values
                # 
				elsif ( $_i->{'EventNo'} eq $self->{'call_alert'}->{'EventNo'} && $_i->{'Status'} == -1 && ( $_i->{'BoxArea'} ne $self->{'call_alert'}->{'BoxArea'} || $_i->{'CallType'} ne $self->{'call_alert'}->{'CallType'} || $_i->{'Location'} ne $self->{'call_alert'}->{'Location'} ) ) 
				{
                    &main::log("[$self->{call_alert}->{EventNo}] ** Pending Incident Update ** [$self->{call_alert}->{BoxArea}||$_i->{BoxArea}] [$self->{call_alert}->{CallType}||$_i->{CallType}] [$self->{call_alert}->{Location}||$_i->{Location}]");

					my $_serial_color;

                    $self->{'serial_message'} = "<amber/>** UPDATE ** ";
					
                    if ( $self->{'alert_controls'}->{'inc_type'} eq 'pending' )
                    {
                    	$_serial_color = 'green';
                    	$_serial_color = 'rainbow1' if $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/;

                    	$self->{'serial_message'} .=
	                    ( $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/ ?
	                        "<$_serial_color/>PENDING FIRST DUE INCIDENT: " : "<$_serial_color/>PENDING INCIDENT: "
	                    );
                    }
					else
					{
	                    $_serial_color = $self->{'call_alert'}->{'DisplayColor'};
	                    $self->{'serial_message'} .= "<$_serial_color />PENDING LOCAL INCIDENT: ";
                    }

                    $self->{'serial_message'} .= "[" . $self->{'call_alert'}->{'BoxArea'} .
                    ( $self->{'call_alert'}->{'BoxArea'} ne $_i->{'BoxArea'} ?
                        "<amber/>=>$_i->{BoxArea}<$_serial_color/>" : ''
                    ) . "] " .
                    ( $self->{'AlertQueueType'}->{ $self->{'call_alert'}->{'CallType'} }->{'Label'} ?
                        $self->{'AlertQueueType'}->{ $self->{'call_alert'}->{'CallType'} }->{'Label'} : $self->{'call_alert'}->{'Label'}
                    ) .
                    ( $self->{'call_alert'}->{'CallType'} ne $_i->{'CallType'} ?
                        "<amber/>=>" .
                        ( $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} ?
                            $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} : $_i->{'CallType'}
                        ) . "<$_serial_color />" : ''
                    ) . " " . encode_entities( $self->{'call_alert'}->{'Location'} ) .
                    ( $self->{'call_alert'}->{'Location'} ne $_i->{'Location'} ?
                        "<amber/>=>" . encode_entities( $_i->{'Location'} ) . "<$_serial_color/>" : ''
                    ) . " <amber/>** UPDATE ** ";

					if ( $_i->{'BoxArea'} ne $self->{'call_alert'}->{'BoxArea'} )
					{
						$self->{'call_alert'}->{'BoxArea'} = $_i->{'BoxArea'};
						$self->{'call_alert'}->{'FullBox'} = $_i->{'FullBox'};
					}

					if ( $_i->{'CallType'} ne $self->{'call_alert'}->{'CallType'} )
					{
						$self->{'call_alert'}->{'CallType'} = $_i->{'CallType'};
						$self->{'call_alert'}->{'Label'} = $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} || $_i->{'Nature'} || $_i->{'CallType'};
						$self->{'call_alert'}->{'Nature'} = $_i->{'Nature'};
					}

					$self->{'call_alert'}->{'Location'} = $_i->{'Location'} if $_i->{'Location'} ne $self->{'call_alert'}->{'Location'};
					$self->{'call_alert'}->{'PreambleTone'}	= $self->{'settings'}->{'ActiveIncUpdateAudioPreamble'},

					undef $self->{'call_alert'}->{'BoxAreaAudio'};
					undef $self->{'call_alert'}->{'TypeAnnounce'};
					
					eval {
                        $self->{'dbh'}->run( sub {
                        	$_->do(
                        		qq{
                        			UPDATE ActiveIncident t1
                                    SET
                                        t1.CallType = ?,
                                        t1.Nature = ?,
                                        t1.BoxArea = ?,
                                        t1.Location = ?,
                                        t1.Update = 1
                                    WHERE t1.EventNo = ?
                        		},
                        		undef,
                        		$_i->{'CallType'},
                        		$_i->{'Nature'},
                        		$_i->{'BoxArea'},
                        		$_i->{'Location'},
                        		$_i->{'EventNo'}
                        	)
                        } )
					};

					if ( my $ex = $@ )
					{
                    	&main::log("Database exception received during active incident update - Unable to update active incident " . $ex->error, E_ERROR);
                    }

                    &main::log("Setting serial message: $self->{serial_message}", E_DEBUG) if $self->{'debug'};

                    $self->{'serial_message'} = "<signboard><text label=\"$self->{LED_TextAddr}\"><mode display=\"" . $self->{'LED_DisplayMode'} . "\"/>$self->{serial_message} </text></signboard>";
                    $self->{'alert_controls'}->{'alert_type'} = 'update';

                    return 1;
                }
	        	elsif ( $_i->{'Status'} == -1 && ( ! $self->{'call_alert'}->{'EventNo'} || $_i->{'EventNo'} ne $self->{'call_alert'}->{'EventNo'} ) ) # NEW PENDING INCIDENT
	        	{
	        		eval {
		                $self->{'dbh'}->run( sub {
		                	$_->do(
			                	qq{
			                		INSERT INTO ActiveIncident
	                                VALUES
	                                (
			                            ?,
		                                CURRENT_TIMESTAMP(),
		                                ?,
		                                ?,
		                                ?,
		                                ?,
		                                ?,
		                                ?,
		                                ?,
		                                0,
		                                0,
    	                                0
	                                )
			                	},
			                	undef,
			                	$_i->{'EventNo'},
    	                        $_i->{'IncidentNo'},
    	                        ( $_i->{'OpenTime'} ? $_i->{'OpenTime'} : undef ),
    	                        ( $_i->{'CloseTime'} ? $_i->{'CloseTime'} : undef ),
    	                        $_i->{'CallType'},
    	                        $_i->{'Nature'},
    	                        $_i->{'BoxArea'},
    	                        $_i->{'Location'}
    	                    )
		                } )
	        		};

	        		if ( my $ex = $@ )
	        		{
                    	&main::log("Database exception received while inserting new active incident - Unable to insert new incident " . $ex->error, E_ERROR) if $ex->err != '1062';
	        		}
	        		else
	        		{
						$self->{'call_alert'} = {};
                        $self->{'alert_controls'} = {
                            'alert_type'	=> 'new',
                            'inc_type'		=> ( defined $self->{'AlertQueueAlert'}->{ $_i->{'Box'} }->{ $_i->{'CallType'} } ? 'pending' : 'firstdue' ),
                            'event_no'		=> $_i->{'EventNo'}
                        };
 
						# New active incident - priority 
						if ( defined $self->{'AlertQueueAlert'}->{ $_i->{'Box'} }->{ $_i->{'CallType'} } )
						{
							$self->{'alert_controls'}->{'alert_class'} = 'pending';
	                        $self->{'call_alert'} = {
	                            EventNo      	=> $_i->{'EventNo'},
	                            CallType       	=> $_i->{'CallType'},
	                            Nature			=> $_i->{'Nature'},
	                            Label			=> $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} || $_i->{'Nature'} || $_i->{'CallType'},
	                            PreambleTone	=> ( $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/ ? $self->{'settings'}->{'ActiveIncAudioPreamble_FirstDue'} : ( $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Priority'} ? $self->{'settings'}->{'ActiveIncAudioPreamble'} : $self->{'settings'}->{'LocalIncAudioPreamble'} ) ),
	                            TypeAnnounce 	=> $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'TypeAnnounce'} || $self->{'CallGroup'}->{ $_i->{'CallGroup'} }->{'AlertAudioAnnounce'},
	                            BoxAreaAudio  	=> $self->{'BoxAreaTTS'}->{ $_i->{'Box'} },
	                            Location       	=> $_i->{'Location'},
	                            BoxArea       	=> $_i->{'BoxArea'},
	                            FullBox   		=> $_i->{'FullBox'},
	                            DisplayColor	=> ( $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/ ? "rainbow1" : "green" )
	                        };

                            # Set the priority flag for reference from main
	                        eval {
	                        	$self->{'dbh'}->run( sub {
	                        		$_->do(
	                        			qq{
	                        				UPDATE ActiveIncident
	                                        SET Priority = 1
	                                        WHERE EventNo = ?
	                        			},
	                        			undef,
	                        			$_i->{'EventNo'}
	                        		)
	                        	} )
	                        };
	                        if ( my $ex = $@ )
	                        {
	                            &main::log("Database exception received when setting active incident priority flag - Unable to flag incident priority " . $ex->error, E_ERROR);
	                        }

						}
						# New active incident - first due
						else
						{
							my $rowref;
							eval {
							    $rowref = $self->{'dbh'}->run( sub {
							    	return $_->selectrow_hashref(
							    		qq{
								            SELECT
								                t1.CallGroup,
								                t1.Label,
								                IFNULL( t1.AlertDisplayColor, t2.AlertDisplayColor ) AS DisplayColor,
								                IFNULL( t1.AlertAudioAnnounce, t2.AlertAudioAnnounce) AS TypeAnnounce
								            FROM CallType t1
								            LEFT JOIN CallGroup t2 ON t2.CallGroup = t1.CallGroup
								            WHERE t1.TypeCode = ?
							    		},
							    		undef,
							    		$_i->{'CallType'}
							    	);
							    } )
							};
	                        if ( my $ex = $@ )
	                        {
	                            &main::log("Database exception received while looking up call type settings - Unable to set call type alert parameters " . $ex->error, E_ERROR);
	                        }

							$self->{'alert_controls'}->{'alert_class'} = 'firstdue';
							$self->{'call_alert'} = {
			                    EventNo       	=> $_i->{'EventNo'},
		                        CallType       	=> $_i->{'CallType'},
		                        Nature			=> $_i->{'Nature'},
		                        Label			=> $rowref->{'Label'} || $_i->{'Nature'} || $_i->{'CallType'},
	                            PreambleTone	=> $self->{'settings'}->{'LocalIncAudioPreamble'},
	                            TypeAnnounce 	=> $rowref->{'TypeAnnounce'} || $self->{'CallGroup'}->{ $_i->{'CallGroup'} }->{'AlertAudioAnnounce'},
	                            BoxAreaAudio  	=> $self->{'BoxAreaTTS'}->{ $_i->{'Box'} },
		                        Location       	=> $_i->{'Location'},
		                        BoxArea			=> $_i->{'BoxArea'},
		                        FullBox   		=> $_i->{'FullBox'},
		                        DisplayColor	=> 'green' #$rowref->{'DisplayColor'} || $self->{'CallGroup'}->{ $_i->{'CallGroup'} }->{'AlertDisplayColor'}
							};
						}

                        &main::log("New " . ( $self->{'alert_controls'}->{'inc_type'} eq 'pending' ? "Pending" : "First Due" ) . " Incident --> #" . $self->{'call_alert'}->{'EventNo'} . " [$self->{call_alert}->{BoxArea}] $self->{call_alert}->{CallType} $self->{call_alert}->{Location}");

						$self->{'call_alert'}->{'DisplayColor'} = 'green' unless ( $self->{'call_alert'}->{'DisplayColor'} );

						# TODO - Add setting to enable/disable notifications for non-priority first due incidents
						$self->{'sms'} = {
							subject	=> "** Pending " .
	                        ( $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/ ?
	                            "First Due " : undef
	                        ) . "Incident ** [$_i->{BoxArea}]",
							message	=>
							( $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} ?
	                            $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} : $self->{'call_alert'}->{'Label'}
	                        ) . " " . $_i->{'Location'}
						};

						if ( $self->{'alert_controls'}->{'inc_type'} eq 'pending' )
						{
	                        $self->{'serial_message'} =
	                        ( $_i->{'Box'} =~ m/^$self->{FirstDueRegex}$/ ?
	                            "<rainbow1 />PENDING FIRST DUE INCIDENT: " : "<green />PENDING INCIDENT: "
	                        );
						}
						else
						{
							$self->{'serial_message'} = "<$self->{call_alert}->{DisplayColor} />PENDING FIRST DUE INCIDENT: ";
						}					

                        $self->{'serial_message'} .= "[$_i->{BoxArea}] " .
                        ( $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} ?
                            $self->{'AlertQueueType'}->{ $_i->{'CallType'} }->{'Label'} : $self->{'call_alert'}->{'Label'}
                        ) . " " . encode_entities( $_i->{'Location'} );

                        &main::log("Setting serial message: $self->{serial_message}", E_DEBUG) if $self->{'debug'};

                        $self->{'serial_message'} = "<signboard><text label=\"$self->{LED_TextAddr}\"><mode display=\"" . $self->{'LED_DisplayMode'} . "\"/>$self->{serial_message} </text></signboard>";

                        return 1;
                    }

                    return undef;
			    }
	        }
	        elsif ( $_i->{'Status'} == 1 && $self->{'call_alert'}->{'EventNo'} eq $_i->{'EventNo'} ) # ACTIVE INCIDENT RESET - DISPATCHED
	        {
                $self->{'call_alert'} = {};
                $self->{'alert_controls'} = {};
                undef $self->{'serial_message'};
                undef $self->{'sms'};
	        }
	    }

        return undef;

    } else {

        &main::log("HTTP request unsuccessful (" . $response->status_line . ")");
		$self->{'req_err'}++;

        # Check for forced logout due to invalid session
        if ( $response->status_line =~ /^30[0-7]/ )
        {
            my $_redirect = $response->header("Location");
            if ( $self->{'settings'}->{'LoginPath'} =~ /^(.*)?$_redirect/ || $_redirect =~ /login/i )
            {
                &main::log("HTTP redirection header to location: $_redirect - Client session is invalid, attempting client re-authentication", E_ERROR);

				if ( $self->init_session(
		        		HostAddr		=> $self->{'settings'}->{'HostAddr'},
		        		LoginPath		=> $self->{'settings'}->{'LoginPath'},
		        		QueryString		=> $self->{'settings'}->{'LoginAddrQueryString'},
		        		LoginHTTP_Post	=> $self->{'settings'}->{'LoginHTTP_Post'},
		        		Username		=> $self->{'settings'}->{'LoginUsername'},
		        		Password		=> $self->{'settings'}->{'LoginPassword'},
		        		FailRegex		=> $self->{'settings'}->{'LoginFailureRegex'}
		        ) )
		        {
				    &main::log("Client authentication successful");
				    return undef;
				}

				&main::log("Client authentication failed", E_ERROR);
            }            
        }

		if ( $self->{'settings'}->{'LogHTTP_Failures'} )
		{
			&main::log("HTTP fail log is enabled, logging HTTP request/response to " . File::Spec->catfile( $self->{'log_dir'}, 'AlertQueue_RequestError.log') );
	        if ( open(HTTP_LOG, ">>" . File::Spec->catfile( $self->{'log_dir'}, 'AlertQueue_RequestError.log') ) )
	        {
	            print HTTP_LOG "[" . POSIX::strftime('%c', localtime) . "] HTTP Request Unsuccessful (" . $response->status_line . ")\n";
	            print HTTP_LOG "HTTP Headers Dump:\n";
	            print HTTP_LOG $response->headers_as_string . "\n\n";
	            print HTTP_LOG "HTTP Body Dump: \n";
	            print HTTP_LOG $response->decoded_content . "\n\n";
	            close HTTP_LOG;
	        }
		}

        return undef;
    }

    return undef;
}

sub validate_cookie
{
    my @params = @_;
    $host_url =~ s/^http:\/\/(.*)/$1/;

    if ( $params[4] eq $host_url && $params[1] eq $cookie_name ) {

        if ( $params[8] && time >= $params[8] ) {

            $valid_session = 0;
        }

        $valid_session = 1;
        return;
    }

    return;
}

sub init_hostsync
{
	my $self = shift;
	my %params = @_;

	unless (
		$self->{'hostsync_dbh'} = DBIx::Connector->new(
			"DBI:$params{DB_Driver}:$params{DB_Name}:$params{DB_Host}:$params{DB_Port}",
	        $params{'DB_User'},
	        $params{'DB_Pass',}
	        {
	        	PrintError	=> 0,
	        	RaiseError	=> 0,
	        	HandleError	=> Exception::Class::DBI->handler,
	    	    AutoCommit	=> 1,
	        }
		)
	) {

		&main::log("HostSync database connection error - Unable to proceed with HostSync configuration " . $DBI::errstr, E_ERROR);
		return undef;
	}

	&main::log("Setting default DBIx mode => 'fixup' ");

	unless ( $self->{'hostsync_dbh'}->mode('fixup') )
	{
		$log->error("Error setting database healing mode " . $DBI::errstr, E_ERROR);
	}

	return;
}
1;