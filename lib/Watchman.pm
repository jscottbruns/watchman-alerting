package Watchman;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use File::Temp qw( tempfile );
use HTML::Entities;
use XML::LibXML;
use Text::Wrap;
use Text::Template;
use Date::Parse;
use Data::Dumper;
use LWP::Simple;

sub new
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;

    &main::log("Instantiating Watchman module");

    my $self = {
    	'path'		=> $::PATH,
	    'debug'		=> $::DEBUG,
    	'temp_dir'	=> $::TEMP_DIR,
	    'dbh'		=> $::DBH,
		'main'		=> $::self,
		'log_dir'	=> $::LOG_DIR,
		'alert'		=> 0,
		'err'		=> undef,
		'errstr'	=> undef,
		'CallGroup'	=> {}
    };

	$self->{'settings'} = {
		'system'		=> $::self->{'system'},
		'ledalerting'	=> $::self->{'ledalerting'},
		'audioalerting'	=> $::self->{'audioalerting'},
		'voicealert'	=> $::self->{'voicealert'}
	};

	# xsltproc (XSLT 1.0) XSLT preprocessor command: [xsltproc %xsl_file -] (default)
	# saxonb-xslt (XSLT 2.0) XSLT preprocessor command: [saxonb-xslt -xsl:%xsl_file -s:-]
	# TODO: Update default XSLTPROC commande
    $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} = $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} || 'xsltproc %xsl_file %source_file';
	$self->{'settings'}->{'system'}->{'DefaultXSL_Path'} = File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'system'}->{'DefaultXSL_Path'} ) if $self->{'settings'}->{'system'}->{'DefaultXSL_Path'};

	unless ( $self->{'settings'}->{'system'}->{'DefaultXSL_Path'} && -f $self->{'settings'}->{'system'}->{'DefaultXSL_Path'} )
	{
		&main::log("Default XSL filepath has not been set in system settings - This error must be corrected in order for incoming dispatch data to be processed - Unable to proceed with incident alerting", E_CRIT) unless ( ! $self->{'settings'}->{'system'}->{'DefaultXSL_Path'} );
		&main::log("Default XSL filepath [ $self->{settings}->{system}->{DefaultXSL_Path} ] cannot be accessed on local filesystem - Please check file path for read privileges - This error must be corrected in order for incoming dispatch data to be processed - Unable to proceed with incident alerting", E_CRIT) unless ( -f $self->{'settings'}->{'system'}->{'DefaultXSL_Path'} );
		return undef;
	}

    $self->{'settings'}->{'ledalerting'}->{'LED_DefaultDisplayColor'} = $self->{'settings'}->{'ledalerting'}->{'LED_DefaultDisplayColor'} || 'red';
	$self->{'settings'}->{'ledalerting'}->{'LED_PreambleDisplayMode'} = $self->{'settings'}->{'ledalerting'}->{'LED_PreambleDisplayMode'} || 'flash';
	$self->{'settings'}->{'ledalerting'}->{'LED_StandardDisplayMode'} = $self->{'settings'}->{'ledalerting'}->{'LED_StandardDisplayMode'} || 'rotate';
	$self->{'settings'}->{'ledalerting'}->{'LED_DefaultTextPage'} = $self->{'settings'}->{'ledalerting'}->{'LED_DefaultTextPage'} || 'A';

	&main::log("Registering station units");

	eval {
		$self->{'units'} = $self->{'dbh'}->run( sub {
			return $_->selectall_hashref(
				q{
					SELECT *
					FROM Units
					WHERE Inactive = 0
				},
				[ 'UnitID' ]
			);
		} )
	};
	if ( my $ex = $@ )
	{
		&main::log("DBI exception during station unit lookup: $ex->error - Unable to perform unit alerting", E_ERROR);
	}

	my $num_units = %{ $self->{units} };

	&main::log("No units found for incident alerting. Proceeding without registered units") unless $num_units;
	&main::log("Registered " . substr($num_units, 0, index($num_units, '/') ) . " station units for incident alerting") if $num_units;

	&main::log("Registering default call groups");

	my $cg;
	eval {
		$cg = $self->{'dbh'}->run( sub {
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

	foreach ( @{ $cg } )
	{
	    $self->{'CallGroup'}->{ $_->{'CallGroup'} } = $_;
	}			

    bless $self, $class;

    unless ( $self->parseDispatch( $params{'data'} ))
    {
        return undef;
    }

    return $self;
}

sub parseDispatch
{
    my $self = shift;
    my $stream = shift;

    &main::log("Parsing input data stream ");

    if ( $self->{'settings'}->{'system'}->{'XSLT_PreProcessCommand'} )
    {
		&main::log("Data preprocessing required - Preparing pre-processing command [ $self->{settings}->{system}->{XSLT_PreProcessCommand} ]");
		$self->{'settings'}->{'system'}->{'XSLT_PreProcessTimeout'} = $self->{'settings'}->{'system'}->{'XSLT_PreProcessTimeout'} || 10;

		if ( $self->{'settings'}->{'system'}->{'XSLT_PreProcessCommand'} =~ /%source_file/ )
		{
			my ($fh, $fname) = tempfile( DIR => $self->{'temp_dir'} );
		   	if ( $fh )
		    {
		    	print $fh $stream;
		    	close $fh;
		    }

			&main::log("Error writing stream data to temporary file for pre-processing: $@ $!", E_ERROR) unless ( $fname );

		    $self->{'settings'}->{'system'}->{'XSLT_PreProcessCommand'} =~ s/%source_file/$fname/g;
		}

		if ( $self->{'settings'}->{'system'}->{'XSLT_PreProcessCommand'} =~ /%temp_file/ )
		{
			my $tempfile = tmpnam();
			$self->{'settings'}->{'system'}->{'XSLT_PreProcessCommand'} =~ s/%temp_file/$tempfile/g;
		}

		$self->{'settings'}->{'system'}->{'XSLT_PreProcessCommand'} =~ s/%xsl_file/$self->{settings}->{system}->{DefaultXSL_Path}/ if ~ m/%xsl_file/;

    	&main::log("Preparing preprocessing command [ $self->{settings}->{system}->{XSLT_PreProcessCommand} ]");

		my $buff = `$self->{settings}->{system}->{XSLT_PreProcessCommand}`;

	   	if ( $buff )
	    {
	    	&main::log("Pre-process command successul, updating stream data with pre-processing output");
	    	$stream = $buff;
	    }
	    else
	    {
	    	&main::log("Runtime error received during XSLT pre-processing - $err ", E_ERROR);
	    }
    }

	if ( $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} =~ /%source_file/ )
	{
		my ($fh, $fname);
	   	if ( ($fh, $fname) = tempfile( DIR => $self->{'temp_dir'} ) )
	    {
	    	print $fh $stream;
	    	close $fh;
	    }

		&main::log("Error writing stream data to temporary file for XSLT processing: $@ $!", E_ERROR) unless ( $fname );

	    $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} =~ s/%source_file/$fname/;
	}

	$self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} =~ s/%xsl_file/$self->{settings}->{system}->{DefaultXSL_Path}/ if ~ /%xsl_file/;
	if ( $self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} =~ /%myunits/ )
	{
		my $_units = join ' ', keys %{ $self->{units} };
		$self->{'settings'}->{'system'}->{'DefaultXSLT_Command'} =~ s/%myunits/$_units/;
	}

	&main::log("Processing dispatch data stream [ $self->{settings}->{system}->{DefaultXSLT_Command} ] ");

	my $buff = `$self->{settings}->{system}->{DefaultXSLT_Command}`;

	if ( $buff )
	{
		my ($fh, $fname) = tempfile(
			'WatchmanIncident_XXXXXX',
			DIR		=> $self->{'log_dir'},
			SUFFIX	=> '.xml'
		);
		if ( $fh )	
		{
			&main::log("Writing XSLT preprocessing XML output => $fname ", E_DEBUG);
			print $fh join '', $buff;
			close $fh;
		}		

    	&main::log("Parsing WatchmanAlerting XML ");

		my $xp;
		eval {
			$xp = XML::LibXML->new->parse_file( $fname )
		};

		&main::log("XML XPATH error - Unable to parse WatchmanXML incident data $! $@", E_ERROR) if ( $@ );

		if ( $xp )
		{			
			my $noteref = [];
			
			foreach my $_comment ( @{ $xp->findnodes('/WatchmanAlerting/IncidentComments/Comment') } )
			{
				push @{ $noteref },
					{
						'EntrySequence'	=> $_comment->findvalue('./EntrySequence'),
						'EntryTime'		=> $_comment->findvalue('./EntryTime'),
						'EntryID'		=> $_comment->findvalue('./EntryID'),
						'EntryFDID' 	=> $_comment->findvalue('./EntryFDID'),
						'Text'			=> $_comment->findvalue('./Text')
					};
			}			
			
			$self->{'dispatch'} = {
				'CallNo'			=> $xp->findnodes('/WatchmanAlerting/EventNo')->to_literal,
				'IncidentNo'		=> $xp->findnodes('/WatchmanAlerting/IncidentNo')->to_literal,
				'ReportNo'			=> $xp->findnodes('/WatchmanAlerting/ReportNo')->to_literal,
				'IncidentStatus'	=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/CurrentStatus')->to_literal,
				'Priority'			=> $xp->findnodes('/WatchmanAlerting/Priority')->to_literal,
				'RadioTac'			=> $xp->findnodes('/WatchmanAlerting/RadioTac')->to_literal,
				'Times'				=>
				{
					'Initiate'		=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/Create')->to_literal,
					'Entry'			=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/Entry')->to_literal,
					'Dispatch'		=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/Dispatch')->to_literal,
					'Enroute'		=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/Enroute')->to_literal,
					'Onscene'		=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/Onscene')->to_literal,
					'Close'			=> $xp->findnodes('/WatchmanAlerting/IncidentStatus/Close')->to_literal,
				},
				'Station'			=> $xp->findnodes('/WatchmanAlerting/IncidentLocation/Station')->to_literal,
				'StationGrid'		=> $xp->findnodes('/WatchmanAlerting/IncidentLocation/StationGrid')->to_literal,
				'BoxArea'			=> $xp->findnodes('/WatchmanAlerting/IncidentLocation/BoxArea')->to_literal,
				'MapGrid'			=> $xp->findnodes('/WatchmanAlerting/IncidentLocation/MapGrid')->to_literal,
				'Location'			=> $self->mapXpath($xp, '/WatchmanAlerting/IncidentLocation/Location/StreetName'),
				'CrossStreet'		=> $self->mapXpath($xp, '/WatchmanAlerting/IncidentLocation/CrossStreet/StreetName'),
				'GPSCoords'			=>
				{
					'Latitude'		=> $xp->findnodes('/WatchmanAlerting/IncidentLocation/GPSCoords/GPSLatitude')->to_literal,
					'Longitude'		=> $xp->findnodes('/WatchmanAlerting/IncidentLocation/GPSCoords/GPSLongitude')->to_literal,
				},
				'CallGroup'			=> $xp->findnodes('/WatchmanAlerting/CallType/CallGroup')->to_literal,
				'CallType'			=> $xp->findnodes('/WatchmanAlerting/CallType/TypeCode')->to_literal,				
				'CallNature'		=> $xp->findnodes('/WatchmanAlerting/CallType/Nature')->to_literal,
				'IncidentNotes'		=> $noteref,
				'FormattedNotes'	=> $self->ConcatComments( $noteref ),
				'UnitDispatch'		=>
				{
					'UnitList'			=> $xp->findnodes('/WatchmanAlerting/UnitDispatch/UnitList')->to_literal, # All units dispatched on this incident, now and before (single space separated string)
					'DispatchTime'		=> $xp->findnodes('/WatchmanAlerting/DispatchDate')->to_literal, # Time of UnitAssign dispatch
					'UnitAssign'		=> $self->mapXpath($xp, '/WatchmanAlerting/UnitDispatch/Assigned'), # Array listing of current dispatch assignment
					'UnitPreempt'		=> [], # Single array list of units removed from incident
					'UnitReplace'		=> {}, # Hash of unit replacements i.e. E828 => MP828 (E828 has been replaced by MP828)
					'UnitBackup'		=> {}, # Hash of unit backups i.e. SAME AS ABOVE
				},
				'VoiceAlert'		=> 
				{
					'EmbeddedAudio'		=> undef,
					'TTSKeyId'			=> $xp->findnodes('/WatchmanAlerting/VoiceAlert/TTSKeyId')->to_literal,						
				},
				'Metadata'			=>
				{
					'DispatchService'	=> $xp->findnodes('/WatchmanAlerting/Metadata/DispatchService')->to_literal,
					'Jurisdiction'		=> $xp->findnodes('/WatchmanAlerting/Metadata/Jurisdiction')->to_literal,
				}
			};
		}
    }
    else
    {
    	&main::log("Runtime error received during XSLT data processing - Unable to process iCAD incident payload $@ $!", E_CRIT);
    	return undef;
    }

	if ( $self->{'settings'}->{'system'}->{'LocationFormatting'} )
	{
		&main::log("Applying formatting rules to incident location");

		$self->{'dispatch'}->{'Location'}->[0] = &main::regex_format( $self->{'dispatch'}->{'Location'}->[0], $self->{'settings'}->{'system'}->{'LocationFormatting'} ) if $self->{'dispatch'}->{'Location'}->[0];
		$self->{'dispatch'}->{'CrossStreet'}->[0] = &main::regex_format( $self->{'dispatch'}->{'CrossStreet'}->[0], $self->{'settings'}->{'system'}->{'LocationFormatting'} ) if $self->{'dispatch'}->{'CrossStreet'}->[0];
		$self->{'dispatch'}->{'CrossStreet'}->[1] = &main::regex_format( $self->{'dispatch'}->{'CrossStreet'}->[1], $self->{'settings'}->{'system'}->{'LocationFormatting'} ) if $self->{'dispatch'}->{'CrossStreet'}->[1];
	}

	&main::log("Initiating dispatch sequence for incident $self->{dispatch}->{IncidentNo} $self->{dispatch}->{CallType} ");

    return $self->NewIncident;
}

sub NewIncident
{
    my $self = shift;

    my @units = $self->filterUnits( $self->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'} );

    &main::log("Checking incident [$self->{dispatch}->{IncidentNo}] for previous event history");

	my $incref;
	eval {
	    $incref = $self->{'dbh'}->run( sub {
			return $_->selectrow_hashref(
				q{
					SELECT
						t1.IncidentNo,
						t1.EventNo,
						t1.Timestamp,
						t1.CallType,
						t1.Nature,
						t1.BoxArea,
						t1.Location,
						GROUP_CONCAT( t2.UnitID SEPARATOR ' ') AS Dispatch,
						t3.CallGroup,
						t3.Label
					FROM Incident t1
					LEFT JOIN IncidentUnit t2 ON t2.IncidentNo = t1.IncidentNo
					LEFT JOIN CallType t3 ON t3.TypeCode = t1.CallType
					WHERE t1.IncidentNo = ?
				},
				undef,
				$self->{'dispatch'}->{'IncidentNo'}
			);
	    } )
	};

	if ( my $ex = $@ )
	{
		&main::log("DBI exception received during prior incident lookup - Proceeding as new incident " . $ex->error, E_ERROR);
	}

	my $prior_incident_flag = undef;
	my $prior_incident;

	unless ( $incref->{'IncidentNo'} ) # No prior results found
	{
		&main::log("No prior event history found, setting alert flag");
		$self->{'alert'} = 1;
	}
	else # Prior incident found
	{
		&main::log("Prior incident history found, comparing incident data");

		$prior_incident_flag = 1;
	    $prior_incident = {
	    	'EventNo'		=> $incref->{'EventNo'},
	    	'IncidentNo'	=> $incref->{'IncidentNo'},
	    	'Timestamp'		=> $incref->{'Timestamp'},
	    	'BoxArea'		=> $incref->{'BoxArea'},
	    	'Location'		=> $incref->{'Location'},
	    	'CallType'		=> $incref->{'CallType'},
	    	'CallNature'	=> $incref->{'Nature'},
	    	'CallGroup' 	=> $incref->{'CallGroup'},
	    	'CallLabel'		=> $incref->{'Label'},
	    	'UnitList'		=> $incref->{'UnitList'},
	    	'UnitAssign'	=> {}
	    };

		$self->{'dispatch'}->{'CallGroup'} = $prior_incident->{'CallGroup'};
		$self->{'dispatch'}->{'CallLabel'} = $prior_incident->{'CallLabel'} || $prior_incident->{'CallNature'} || $prior_incident->{'CallType'};

	    eval {
	    	$prior_incident->{'UnitAssign'} = $self->{'dbh'}->run( sub {
	    		return $_->selectall_hashref(
	    			qq{
	    				SELECT
	    					UnitID,
	    					DispatchTime,
	    					Cancelled
	    				FROM IncidentUnit
	    				WHERE IncidentNo = ?
	    			},
	    			[ 'UnitID' ],
	    			undef,
	    			$self->{'dispatch'}->{'IncidentNo'}
	    		);
	    	} )
	    };
	    if ( my $ex = $@ )
	    {
	    	&main::log("Database exception during previous incident unit history retrieval - Unable to access previous unit history for incident [$self->{dispatch}->{IncidentNo}] " . $ex->error, E_ERROR);
	    }

		&main::log("Checking current unit dispatch assignment for unit replacements, backups, cancellations");

		foreach my $_unit ( @units )
		{
			if ( $prior_incident->{'UnitAssign'}->{ $_unit } )
			{
				if ( $self->{'dispatch'}->{'UnitDispatch'}->{'UnitReplace'}->{ $_unit } && $prior_incident->{'UnitAssign'}->{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitReplace'}->{ $_unit } } ) # Replaced unit assigned during previous dispatch event
				{
					&main::log("  Unit replacement $self->{dispatch}->{UnitDispatch}->{UnitReplace}->{ $_unit } --> $_unit ");

					$self->{'silent'} = 1;
					eval {
						$self->{'dbh'}->run( sub {
							$_->do(
								qq{
									UPDATE IncidentUnit
									SET
										UnitID = ?,
										DispatchTime = ?
									WHERE IncidentNo = ? AND UnitID = ?
								},
								undef,
								$_unit,
								$self->{'dispatch'}->{'DispatchTime'},
								$self->{'dispatch'}->{'IncidentNo'},
								$self->{'dispatch'}->{'UnitDispatch'}->{'UnitReplace'}->{ $_unit }
							);
						} )
					};
					if ( my $ex = $@ )
					{
						&main::log("Database exception received when updating unit history for incident [$self->{dispatch}->{IncidentNo}] - Unable to reflect unit replacement in incident unit history " . $ex->error, E_ERROR);
					}
				}
				else
				{
					&main::log("  Previous unit assignment [ $_unit ] Alerting not required");

					my $_i = &main::indexArray( $_unit, @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'} } );
					if ( $_i >= 0 )
					{
						splice @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'} }, $_i, 1;
					}
				}
			}
			elsif ( ! $prior_incident->{'UnitAssign'}->{ $_unit } )  # Replaced unit assigned and immediately reversed, prior to initial dispatch
			{
				$self->{'alert'} = 1;
				$self->{'NewUnitFlag'} = 1;

				eval {
					$self->{'dbh'}->run( sub {
						$_->do(
							qq{
								INSERT INTO IncidentUnit
								( IncidentNo, UnitID, DispatchTime )
								VALUES ( ?, ?, ? )
							},
							undef,
							$self->{'dispatch'}->{'IncidentNo'},
							$_unit,
							$self->{'dispatch'}->{'DispatchTime'}
						);
					} )
				};
				if ( my $ex = $@ )
				{
					&main::log("Database exception received when updating unit history for incident [$self->{dispatch}->{IncidentNo}] - Unable to assign dispatched unit [$_unit] " . $ex->error, E_ERROR);
				}
			}
		}

		if ( @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitPreempt'} } )
		{
			foreach my $_unit ( @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitPreempt'} } )
			{
				if ( $prior_incident->{'UnitAssign'}->{ $_unit } && ! $prior_incident->{'UnitAssign'}->{ $_unit }->{'Cancelled'} )
				{
					&main::log("Updating incident unit history for unit cancellation --> $_unit");
					$self->{'dispatch'}->{'PreemptFlag'} = 1;

					eval {
						$self->{'dbh'}->run( sub {
							$_->do(
								qq{
									UPDATE IncidentUnit
									SET Cancelled = 1
									WHERE IncidentNo = ? AND UnitID = ?
								},
								undef,
								$self->{'dispatch'}->{'IncidentNo'},
								$_unit
							);
						} )
					};

					if ( my $ex = $@ )
					{
						&main::log("Database exception received when updating unit history for incident [$self->{dispatch}->{IncidentNo}] - Unable to set unit cancellation flag for cancelled unit [$_unit] " . $ex->error, E_ERROR);
					}
				}
			}
		}
	}

	undef $self->{'silent'} if $self->{'NewUnitFlag'};

    if ( $self->{'dispatch'}->{'IncidentStatus'} eq '0' || $self->{'dispatch'}->{'IncidentStatus'} =~ /^(CLOSED|CANCELLED)$/ )
    {
		&main::log("Current incident status is [ $self->{dispatch}->{IncidentStatus} ] - Alerting is not required at this time");

		$self->{'dispatch'}->{'printout'} = $self->WritePrintout;

		undef $self->{'dispatch'}->{'alert'};
		undef $self->{'dispatch'}->{'silent'};

		return 1;
	}

	if ( $self->{'alert'} || $self->{'silent'} || $self->{'dispatch'}->{'PreemptFlag'} )
	{
		if ( $prior_incident_flag || $self->{'dispatch'}->{'PreemptFlag'} ) # If previous dispatch then merge with new dispatch
		{
			&main::log("Updating incident parameters for incident [$self->{dispatch}->{IncidentNo}]");

			eval {
				$self->{'dbh'}->run( sub {
	                $_->do(
	                	qq{
							UPDATE Incident
		                	SET
		                		ReportNo = ?,
		                		EnrouteTime = ?,
		                		OnsceneTime = ?,
		                		CloseTime = ?,
		                		CallType = ?,
		                		Nature = ?,
		                		Station = ?,
		                		BoxArea = ?,
		                		Location = ?,
		                		LocationNote = ?,
		                		CrossSt1 = ?,
		                		CrossSt2 = ?,
		                		GPSLatitude = ?,
		                		GPSLongitude = ?,
		                		UnitList = ?,
		                		Comments = ?
		                	WHERE IncidentNo = ?
	                	},
	                	undef,
	                	$self->{'dispatch'}->{'ReportNo'},
	                	( $self->{'dispatch'}->{'Times'}->{'Enroute'} ? $self->{'dispatch'}->{'Times'}->{'Enroute'} : undef ),
	                	( $self->{'dispatch'}->{'Times'}->{'Onscene'} ? $self->{'dispatch'}->{'Times'}->{'Onscene'} : undef ),
	                	( $self->{'dispatch'}->{'Times'}->{'Close'} ? $self->{'dispatch'}->{'Times'}->{'Close'} : undef ),
	                	$self->{'dispatch'}->{'CallType'},
	                	$self->{'dispatch'}->{'CallNature'},
	                	$self->{'dispatch'}->{'Station'},
	                	$self->{'dispatch'}->{'BoxArea'},
	                	$self->{'dispatch'}->{'Location'}->[0],
	                	$self->{'dispatch'}->{'Location'}->[1],
	                	$self->{'dispatch'}->{'CrossStreet'}->[0],
	                	$self->{'dispatch'}->{'CrossStreet'}->[1],
	                	$self->{'dispatch'}->{'GPSCoords'}->{'Latitude'},
	                	$self->{'dispatch'}->{'GPSCoords'}->{'Longitude'},
	                	$self->{'dispatch'}->{'UnitDispatch'}->{'UnitList'},
	                	$self->{'dispatch'}->{'FormattedNotes'},
	                	$self->{'dispatch'}->{'IncidentNo'}
	                );
				} )
			};
			if ( my $ex = $@ )
			{
				&main::log("Database exception received during prior incident unit update: ". $ex->error, E_ERROR);
			}

			$self->{'dispatch'}->{'printout'} = $self->WritePrintout;

			return 1 if ( $self->{'dispatch'}->{'PreemptFlag'} && ! $self->{'alert'} && ! $self->{'silent'} );
		}
		else
		{
			&main::log("Committing incident [$self->{dispatch}->{IncidentNo}] to master incident database");

			eval {
				$self->{'dbh'}->run( sub {
					$_->do(
						qq{
        	            	INSERT INTO Incident
			                VALUES
			                (
					            ?, # EventNo
					            ?, # IncidentNo
					            ?, # ReportNo
					            CURRENT_TIMESTAMP(),
					            ?, # EntryTime
					            ?, # OpenTime
				    	        ?, # DispatchTime
				        	    ?, # EnrouteTime
				            	?, # OnsceneTime
					            ?, # CloseTime
					            ?, # CallType
					            ?, # Nature
					            ?, # Station
					            ?, # BoxArea
					            ?, # Location
					            ?, # LocationNote
					            ?, # CrossSt1
					            ?, # CrossSt2
					            ?, # GPSLatitude
					            ?, # GPSLongitude
					            ?, # UnitList
					            ?  # Comments
			                )
						},
			            undef,
			            $self->{'dispatch'}->{'CallNo'},
			            $self->{'dispatch'}->{'IncidentNo'},
			            $self->{'dispatch'}->{'ReportNo'},
			            ( $self->{'dispatch'}->{'Times'}->{'Entry'} ? $self->{'dispatch'}->{'Times'}->{'Entry'} : undef ),
			            ( $self->{'dispatch'}->{'Times'}->{'Initiate'} ? $self->{'dispatch'}->{'Times'}->{'Initiate'} : undef ),			            
			            ( $self->{'dispatch'}->{'Times'}->{'Dispatch'} ? $self->{'dispatch'}->{'Times'}->{'Dispatch'} : undef ),
			            ( $self->{'dispatch'}->{'Times'}->{'Enroute'} ? $self->{'dispatch'}->{'Times'}->{'Enroute'} : undef ),
			            ( $self->{'dispatch'}->{'Times'}->{'Onscene'} ? $self->{'dispatch'}->{'Times'}->{'Onscene'} : undef ),
			            ( $self->{'dispatch'}->{'Times'}->{'Close'} ? $self->{'dispatch'}->{'Times'}->{'Close'} : undef ),
			            $self->{'dispatch'}->{'CallType'},
			            $self->{'dispatch'}->{'CallNature'},
			            $self->{'dispatch'}->{'Station'},
			            $self->{'dispatch'}->{'BoxArea'},
			            $self->{'dispatch'}->{'Location'}->[0],
			            $self->{'dispatch'}->{'Location'}->[1],
			            $self->{'dispatch'}->{'CrossStreet'}->[0],
			            $self->{'dispatch'}->{'CrossStreet'}->[1],
			            $self->{'dispatch'}->{'GPSCoords'}->{'Latitude'},
			            $self->{'dispatch'}->{'GPSCoords'}->{'Longitude'},
			            $self->{'dispatch'}->{'UnitDispatch'}->{'UnitList'},
			            &main::trim( $self->{'dispatch'}->{'FormattedNotes'} )
					);
				} )
			};
			if ( my $ex = $@ )
			{
				&main::log("Database exception received during incident insert - Unable to add new incident to database " . $ex->error, E_ERROR);
			}

			&main::log("Saving station unit dispatch assignments");

			foreach my $_unit ( @units )
			{
				eval {
					$self->{'dbh'}->run( sub {
						$_->do(
							qq{
	        	            	INSERT INTO IncidentUnit
	        	            	( IncidentNo, UnitID, DispatchTime )
				                VALUES ( ?, ?, ? )
							},
							undef,
							$self->{'dispatch'}->{'IncidentNo'},
							$_unit,
							$self->{'dispatch'}->{'UnitDispatch'}->{'DispatchTime'}
						);
					} )
				};
				if ( my $ex = $@ )
				{
					&main::log("Database exception received during station unit insert - Unable to save station unit [$_unit} for incident [$self->{dispatch}->{Incident}]" . $ex->error, E_ERROR);
				}
			}
		}

		return $self->activate;
	}

	$self->{'dispatch'}->{'printout'} = $self->WritePrintout;
	&main::log("Existing incident [$self->{dispatch}->{IncidentNo} does not require alerting at this time");

	return 1;
}

##########################################################################
##                                                                      ##
## Assign the required activation variables for audible/visual alerting ##
##                                                                      ##
## @param object @self                                                  ##
## @return bool                                                         ##
##########################################################################

sub activate
{
    my $self = shift;

    &main::log("Activating incident alerting for incident $self->{dispatch}->{IncidentNo}");

    my (@unitAssignment, $priority, $displayColor, $unitAssignDisplay, $title, $message, $flashSpeed, $scrollSpeed);

	$flashSpeed = "<$self->{settings}->{ledalerting}->{LED_PreambleDisplaySpeed}/>" if $self->{'settings'}->{'ledalerting'}->{'LED_PreambleDisplaySpeed'};
	$scrollSpeed = "<$self->{settings}->{ledalerting}->{LED_StandardDisplaySpeed}/>" if $self->{'settings'}->{'ledalerting'}->{'LED_StandardDisplaySpeed'};

	my $calltype;
	eval {
		$calltype = $self->{'dbh'}->run( sub {
			return $_->selectrow_hashref(
				qq{
					SELECT
						t1.Label,
						t1.Priority,
						t1.CallGroup,
						IFNULL( t1.AlertDisplayColor, t2.AlertDisplayColor ) AS AlertDisplayColor,
						IFNULL( t1.AlertAudioAnnounce, t2.AlertAudioAnnounce ) AS AlertAudioAnnounce,
						t2.AudioPreAmble,
						t2.AudioPostAmble
					FROM CallType t1
					LEFT JOIN CallGroup t2 ON t1.CallGroup = t2.CallGroup
					WHERE t1.TypeCode = ?
				},
				undef,
				$self->{'dispatch'}->{'CallType'}
			);
		} )
	};

	if ( my $ex = $@ )
	{
		&main::log("Database exception received during call type lookup - Unable to fetch alerting parameters for call type $self->{dispatch}->{CallType}: " . $ex->error, E_ERROR);
	}

	$self->{'dispatch'}->{'CallLabel'} = $self->{'dispatch'}->{'CallNature'} || $self->{'dispatch'}->{'CallType'} unless $self->{'dispatch'}->{'CallLabel'};
	
	my $led_flash = $self->{'settings'}->{'ledalerting'}->{'LED_PreambleMessage'};
	my $led_scroll = $self->{'settings'}->{'ledalerting'}->{'LED_ScrollingMessage'};
	
    if ( $calltype || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} } )
    {
	    $self->{'dispatch'}->{'CallGroup'} = $calltype->{'CallGroup'} if $calltype->{'CallGroup'};
	    $self->{'dispatch'}->{'CallLabel'} = $calltype->{'Label'} if $calltype->{'Label'};

        &main::log("Call type identified => $self->{dispatch}->{CallLabel} ($self->{dispatch}->{CallGroup})");

        # If the incident is in the first due and it's a priority call
        if ( $self->{'dispatch'}->{'Station'} eq $self->{'settings'}->{'system'}->{'DefaultStationID'} && $calltype->{'Priority'} )
        {
            &main::log("Priority incident identified, setting priority flag");
            
            $priority = 1;

            $displayColor = "<$self->{settings}->{ledalerting}->{LED_PriorityDisplayColor} />";
            $title = "<mode display=\"$self->{settings}->{ledalerting}->{LED_PreambleDisplayMode}\"/>" . $flashSpeed . $displayColor;
            $led_flash = encode_entities( $self->{'settings'}->{'ledalerting'}->{'LED_PriorityPreambleDisplay'} ) if $self->{'settings'}->{'ledalerting'}->{'LED_PriorityPreambleDisplay'};                      
        } 
        else 
        {
        	$displayColor = $calltype->{'AlertDisplayColor'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AlertDisplayColor'} || $self->{'settings'}->{'ledalerting'}->{'LED_DefaultDisplayColor'};
        	$displayColor = "<$displayColor/>";
            $title = "<mode display=\"$self->{settings}->{ledalerting}->{LED_PreambleDisplayMode}\"/>" . $flashSpeed . $displayColor;
        }
    }
    else
    {
        &main::log("Failed to lookup call type, applying generic settings", E_WARN);

	    $self->{'dispatch'}->{'CallGroup'} = undef;
	    $self->{'dispatch'}->{'CallLabel'} = $self->{'dispatch'}->{'CallNature'} || $self->{'dispatch'}->{'CallType'};
	    
        $displayColor = "<$self->{settings}->{ledalerting}->{LED_DefaultDisplayColor} />";
        $title = "<mode display=\"$self->{settings}->{ledalerting}->{LED_PreambleDisplayMode}\"/>" . $flashSpeed . $displayColor;
    }	

	my $led = $self->formatLedDisplay( {
		'title'		=> $led_flash,
		'scroll'	=> $led_scroll
	} );

	$led->{'title'} = $title . $led->{'title'}; 
	&main::log("Setting LED display preamble: $led->{title}");
	
    $led->{'scroll'} = "<mode display=\"$self->{settings}->{ledalerting}->{LED_StandardDisplayMode}\"/>" . $scrollSpeed . $displayColor . $led->{'scroll'};
    &main::log("Setting LED display message: $led->{scroll}");

    my $beep;
    
    if ( $self->{'settings'}->{'ledalerting'}->{'LED_BeepEnabled'} )
    {
    	&main::log("LED display beep enabled, setting beep parameters");

    	my $beep_type = "type=\"$self->{settings}->{ledalerting}->{LED_BeepType}\"" if $self->{'settings'}->{'ledalerting'}->{'LED_BeepType'};
    	$beep = "<beep $beep_type/>";
    }

    $self->{'dispatch'}->{'LED_Message'} = 
    {
    	'title'		=> "<signboard>$beep<text label=\"$self->{settings}->{ledalerting}->{LED_DefaultTextPage}\">$led->{title}</text></signboard>",
    	'message'	=> "<signboard><text label=\"$self->{settings}->{ledalerting}->{LED_DefaultTextPage}\">$led->{scroll} </text></signboard>"
    };

    $self->{'dispatch'}->{'printout'} = $self->WritePrintout;

    return 1 if $self->{'silent'};	

    if ( $self->{'settings'}->{'audioalerting'}->{'NetAudioEnabled'} )
    {
        &main::log("Preparing audible alerting");

        $self->{'dispatch'}->{'audiosrc'} = [];

		# Preamble audio tone
		if ( $priority )
		{
			$self->{'dispatch'}->{'priority'} = 1 if $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPreamble'};
			
			push @{ $self->{'dispatch'}->{'audiosrc'} }, $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPreamble'} || $calltype->{'AudioPreAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPreAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPreamble'};
			&main::log("Setting priority audio preamble => [" . ( $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPreamble'} || $calltype->{'AudioPreAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPreAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPreamble'} ) . "]");			
		}
		else
		{
			push @{ $self->{'dispatch'}->{'audiosrc'} }, $calltype->{'AudioPreAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPreAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPreamble'};
			&main::log("Setting priority audio preamble => [ " . ( $calltype->{'AudioPreAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPreamble'} ) . "]");
		}

		# Unit audio announcement
		goto TYPE_ANNOUNCE if ( $priority && $self->{'settings'}->{'audioalerting'}->{'PrioritySkipUnitAnnounce'} );

        foreach ( @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'} } )
        {
	        if ( $self->{'units'}->{ $_ }->{'AudioAnnounce'} )
	        {
	            &main::log("Assigning unit audio announcement $_ => $self->{units}->{ $_ }->{AudioAnnounce}");
                push @{ $self->{'dispatch'}->{'audiosrc'} }, $self->{'units'}->{ $_ }->{'AudioAnnounce'};
            }
        }

		# Call type audio announcement
		TYPE_ANNOUNCE:
		goto TTS_ANNOUNCE if ( $priority && $self->{'settings'}->{'audioalerting'}->{'PrioritySkipTypeAnnounce'} );

		if ( ( $calltype->{'AlertAudioAnnounce'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AlertAudioAnnounce'} ) && ( ! $ self->{'settings'}->{'voicealert'}->{'TTS_Enabled'} || ! $self->{'dispatch'}->{'VoiceAlert'}->{'TTSKeyId'} ) )
		{
	        &main::log("Assigning call type audio announcement $self->{dispatch}->{CallType} => $calltype->{AlertAudioAnnounce}");
            push @{ $self->{'dispatch'}->{'audiosrc'} }, $calltype->{'AlertAudioAnnounce'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AlertAudioAnnounce'};
		}

		TTS_ANNOUNCE:
		if ( $self->{'settings'}->{'voicealert'}->{'TTS_Enabled'} )
		{
			&main::log("VoiceAlert TTS audible dispatch service enabled, checking for VoiceAlert dispatch");
			
			if ( $self->{'dispatch'}->{'VoiceAlert'}->{'TTSKeyId'} )
			{
				&main::log("VoiceAlert conversion key exists - Initiating fetch process for TTS key [$self->{dispatch}->{VoiceAlert}->{TTSKeyId}]");
				
				my $uri = $self->{'settings'}->{'voicealert'}->{'TTS_URI'};
				$uri =~ s/%conversionid/$self->{dispatch}->{VoiceAlert}->{TTSKeyId}/;

				&main::log("Initiating status request to host [$uri]");

				my ($tts_fh, $tts_fname) = tempfile(
					'TTS_Stat_XXXXX',
					DIR		=> $self->{'temp_dir'},
					SUFFIX	=> '.txt'
				);
				
				if ( $tts_fname )
				{
					&main::log("Submitting WGET request to TTS status URI: [$uri] => $tts_fname");
					
					my ($wav_fh, $wav_fname) = tempfile(
						'ttswav_XXXXX',
						DIR		=> $self->{'temp_dir'},
						SUFFIX	=> '.wav'
					);								
					
					if ( $wav_fname )
					{						
						push @{ $self->{'dispatch'}->{'audiosrc'} }, $wav_fname;
					}
					
					if ( ! ( fork ) )
					{
						`wget --output-document=$tts_fname "$uri"`;
								
						if ( open(FH, "<$tts_fname") )
						{
							my $resp = <FH>;
							close FH;
						
							my ($tts_url, $tts_status, $tts_statmsg);
							
							$resp =~ /statusCode="([0-9])"/ and $tts_status = $1;
							$resp =~ /status="(.*?)"/ and $tts_statmsg = $1;
							$resp =~ /downloadUrl="(.*?)"/ and $tts_url = $1;
							
							if ( $tts_status == 4 && $tts_url ) 
							{								
								if ( $wav_fname )
								{
									&main::log("TTS conversion status successful, initiating content download from [$tts_url] => [$wav_fname]");
									`wget --output-document=$wav_fname "$tts_url"`;
								}
								else
								{
									&main::log("Error creating temporary file for content download $@ $!", E_ERROR);	
								}
							}
							else
							{
								&main::log("Error during TTS status check - Status Result: [$tts_status] Unable to proceed with VoiceAlert", E_ERROR);
							}
						}
						else
						{
							&main::log("Unable to read TTS status response - Can't open temp file [$tts_fname]", E_ERROR);	
						}
						
						exit(0);
					}	
				}													
			}
			else
			{
				&main::log("VoiceAlert key/URI not found, skipping VoiceAlert TTS", E_ERROR); 	
			}				  					
		}

		# Postamble audio tone
		POSTAMBLE:
		if ( $priority )
		{
			if ( $self->{'settings'}->{'audioalerting'}->{'PrioritySkipPostAmble'} )
			{
				push @{ $self->{'dispatch'}->{'audiosrc'} }, $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'} if $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'};
				&main::log("Setting priority audio postamble => [$self->{settings}->{audioalerting}->{PriorityAudioPostamble}]") if $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'};
			}
			else
			{
				push @{ $self->{'dispatch'}->{'audiosrc'} }, $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'} || $calltype->{'AudioPostAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} if ( $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'} || $calltype->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} );
				&main::log("Setting priority audio postamble => [" . ( $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'} || $calltype->{'AudioPostAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} ) . "]") if ( $self->{'settings'}->{'audioalerting'}->{'PriorityAudioPostamble'} || $calltype->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} );
			}
		}
		else
		{
			push @{ $self->{'dispatch'}->{'audiosrc'} }, $calltype->{'AudioPostAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} if ( $calltype->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} );
			&main::log("Setting audio postamble => [" . ( $calltype->{'AudioPostAmble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} ) . "]") if ( $calltype->{'AudioPostAmble'} || $self->{'settings'}->{'audioalerting'}->{'DefaultAudioPostamble'} || $self->{'CallGroup'}->{ $self->{'dispatch'}->{'CallGroup'} }->{'AudioPostAmble'} );
		}
    }

    return 1;
}

#####################################################################
##                                                                 ##
## Compare full dispatch assignment against units assigned to our  ##
## station, removing those units not belonging to our station      ##
##                                                                 ##
## @param object $self                                             ##
## @return array @unitAssignment                                   ##
##                                                                 ##
#####################################################################

sub filterUnits
{
	my $self = shift;
	my $unitAssign = shift;

	if ( $unitAssign )
	{
        my @unitAssignment;
		foreach my $_u ( @{ $unitAssign } )
		{
		    push @unitAssignment, $_u if $self->{'units'}->{ $_u };
		}

	    return @unitAssignment;
	}
}

sub mapXpath
{
	my $self = shift;
	my $xp = shift;
	my $path = shift;
	my $array = [];

	for my $node ( $xp->findnodes( $path ) )
	{	
		push @{ $array }, $node->to_literal;
	}

	return $array;
}

sub ConcatComments
{
	my $self = shift;
	my $notes = shift;

	my ($fdid, $entryid);
	$fdid = 1 if length( &main::trim($notes->[0]->{'EntryFDID'}) ) > 0;
	$entryid = 1 if length( &main::trim($notes->[0]->{'EntryID'}) ) > 0;

	my $colsize = 72;
	$colsize += 5 unless ( $fdid );
	$colsize += 9 unless ( $entryid );

	my $packsize = 23;
	$packsize -= 5 unless ( $fdid );
	$packsize -= 9 unless ( $entryid );

	$Text::Wrap::columns = $colsize;
	$Text::Wrap::separator = "\n" . pack("A$packsize", '');

	my $notestr;
	foreach my $_i ( @{ $notes } )
	{
		$_i->{'Text'} =~ s/(\[)(.*?)(\])/$2/g;
		
		$notestr .=
		( $_i->{'EntryTime'} ?
			POSIX::strftime('%H:%M:%S ', localtime( Date::Parse::str2time( $_i->{'EntryTime'} ) ) ) : pack("A9", '')
		) .
		( $fdid ?
			pack("A4", &main::trim($_i->{'EntryFDID'})) . ' ' : undef
		) .
		( $entryid ?
			pack("A8", &main::trim($_i->{'EntryID'})) . ' ' : undef
		) .
		wrap('', '', &main::trim( $_i->{'Text'} ) ) . "\n";
	}

	return $notestr;
}

sub WritePrintout
{
	my $self = shift;

	&main::log("Assembling incident print view using template [$self->{settings}->{system}->{DefaultTemplatePath}]");

	$Text::Wrap::columns = 60;
	$Text::Wrap::separator = "\n" . pack("A10", '');

	if ( $self->{'settings'}->{'system'}->{'DefaultTemplatePath'} && -f File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'system'}->{'DefaultTemplatePath'} ) )
	{
		if (
			my $template = Text::Template->new(
				TYPE	=> 'FILE',
				SOURCE	=> File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'system'}->{'DefaultTemplatePath'} )
			)
		)
		{
			&main::log("Processing printable dispatch incident template");

			my $printview = $template->fill_in(
				HASH	=> {
					EventNo			=> $self->{'dispatch'}->{'IncidentNo'},
					InitTime		=> ( $self->{'dispatch'}->{'Times'}->{'Initiate'} ? pack("A20", POSIX::strftime('%H:%M:%S  %m/%d/%y', localtime( Date::Parse::str2time($self->{'dispatch'}->{'Times'}->{'Initiate'}, "%s") ) ) ) : pack("A20", '') ),
					IncidentNo		=> $self->{'dispatch'}->{'ReportNo'},
					AlarmLevel		=> $self->{'dispatch'}->{'AlarmLevel'},
					CurrentStatus	=> $self->{'dispatch'}->{'IncidentStatus'},
					EntryTime		=> ( $self->{'dispatch'}->{'Times'}->{'Entry'} ? pack("A20", POSIX::strftime('%H:%M:%S', localtime( Date::Parse::str2time($self->{'dispatch'}->{'Times'}->{'Entry'}, "%s") ) ) ) : pack("A20", '') ),
					DispatchTime	=> ( $self->{'dispatch'}->{'Times'}->{'Dispatch'} ? pack("A20", POSIX::strftime('%H:%M:%S', localtime( Date::Parse::str2time($self->{'dispatch'}->{'Times'}->{'Dispatch'}, "%s") ) ) ) : pack("A20", '') ),
					OnsceneTime		=> ( $self->{'dispatch'}->{'Times'}->{'Onscene'} ? pack("A20", POSIX::strftime('%H:%M:%S', localtime( Date::Parse::str2time($self->{'dispatch'}->{'Times'}->{'Onscene'}, "%s") ) ) ) : pack("A20", '') ),
					CloseTime		=> ( $self->{'dispatch'}->{'Times'}->{'Close'} ? pack("A20", POSIX::strftime('%H:%M:%S', localtime( Date::Parse::str2time($self->{'dispatch'}->{'Times'}->{'Close'}, "%s") ) ) ) : pack("A20", '') ),
					Location		=> $self->{'dispatch'}->{'Location'}->[0],
					CrossStreet		=> ( $self->{'dispatch'}->{'CrossStreet'}->[0] || $self->{'dispatch'}->{'CrossStreet'}->[1] ? "(" . join(' & ', @{ $self->{'dispatch'}->{'CrossStreet'} } ) . ")" : undef ),
					LocationNote	=> $self->{'dispatch'}->{'Location'}->[1],
					MapGrid			=> $self->{'dispatch'}->{'MapGrid'},
					Nature			=> $self->{'dispatch'}->{'CallLabel'},
					CallType		=> $self->{'dispatch'}->{'CallType'},
					Station			=> pack("A9", $self->{'dispatch'}->{'Station'}),
					Priority		=> $self->{'dispatch'}->{'Priority'},
					BoxArea			=> pack("A9", $self->{'dispatch'}->{'BoxArea'}),
					StationGrid		=> $self->{'dispatch'}->{'StationGrid'},
					Assigned		=> join(' ', @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'} }),
					UnitList		=> wrap('', '', $self->{'dispatch'}->{'UnitDispatch'}->{'UnitList'}),
					Narrative		=> $self->{'dispatch'}->{'FormattedNotes'}
				}
			);

			if ( $self->{'debug'} )
			{
				if (
					my ($fh, $fname) = tempfile(
						'IncidentPrintout_XXXXXX',
						DIR		=> $self->{'log_dir'},
						SUFFIX	=> '.txt'
					)
				)
				{
					&main::log("Writing print view template output => $fname ", E_DEBUG);
					print $fh $printview;
					close $fh;
				}
			}

			return $printview;
		}
		else
		{
			&main::log("Error launching template object - Unable to create incident print view " . $Text::Template::ERROR, E_ERROR);
			return undef;
		}
	}

	&main::log("Print view template is not set - Can't assemble incident print view") unless ( $self->{'settings'}->{'system'}->{'DefaultTemplatePath'} );
	&main::log("Can't access print view template on local filesystem [ $self->{settings}->{system}->{DefaultTemplatePath} ] Please check system settings for valid print view template") unless ( -f File::Spec->catfile( $self->{'path'}, $self->{'settings'}->{'system'}->{'DefaultTemplatePath'} ) );
}

sub formatLedDisplay
{
	my $self = shift;
	my $led = shift;	
	
    my ($status, $unitassg, $xstreet);
    
    $status = 'Pending' if ( $self->{'dispatch'}->{'IncidentStatus'} eq '-1' );
    $status = 'Dispatched' if ( $self->{'dispatch'}->{'IncidentStatus'} eq '1' );
    $status = 'Enroute' if ( $self->{'dispatch'}->{'IncidentStatus'} eq '2' );
    $status = 'Onscene' if ( $self->{'dispatch'}->{'IncidentStatus'} eq '3' );
    $status = 'Closed' if ( $self->{'dispatch'}->{'IncidentStatus'} eq '0' );

	foreach ( @{ $self->{'dispatch'}->{'UnitDispatch'}->{'UnitAssign'} } )
    {
    	$unitassg .= ( $unitassg ? "/" : undef );
    	$unitassg .= encode_entities( $self->{'units'}->{ $_ }->{'UnitLabel'} ) if ( $self->{'units'}->{ $_ }->{'UnitLabel'} );
    	$unitassg .= encode_entities( $_ ) unless ( $self->{'units'}->{ $_ }->{'UnitLabel'} );    	
	}
	
	$xstreet = "($self->{'dispatch'}->{'CrossStreet'}->[0] & $self->{'dispatch'}->{'CrossStreet'}->[1])" if ( $self->{'dispatch'}->{'CrossStreet'}->[0] && $self->{'dispatch'}->{'CrossStreet'}->[1] );
	$xstreet = "(near $self->{'dispatch'}->{'CrossStreet'}->[0])" if ( $self->{'dispatch'}->{'CrossStreet'}->[0] && ! $self->{'dispatch'}->{'CrossStreet'}->[1] );
	$xstreet = "(near $self->{'dispatch'}->{'CrossStreet'}->[1])" if ( $self->{'dispatch'}->{'CrossStreet'}->[1] && ! $self->{'dispatch'}->{'CrossStreet'}->[0] );
	
    my $regex = {
        '%CallNo'           => encode_entities( $self->{'dispatch'}->{'CallNo'} ),
        '%IncidentNo'       => encode_entities( $self->{'dispatch'}->{'IncidentNo'} ),
        '%IncidentStatus'   => $status,
        '%Priority'         => encode_entities( $self->{'dispatch'}->{'Priority'} ),
        '%RadioTac'         => encode_entities( $self->{'dispatch'}->{'RadioTac'} ),
        '%Station'          => encode_entities( $self->{'dispatch'}->{'Station'} ),
        '%BoxArea'          => encode_entities( $self->{'dispatch'}->{'BoxArea'} ),
        '%MapGrid'          => encode_entities( $self->{'dispatch'}->{'MapGrid'} ),
        '%Location'         => encode_entities( $self->{'dispatch'}->{'Location'}->[0] ),
        '%CrossStreet'		=> encode_entities( $xstreet ),
        '%Latitude'         => encode_entities( $self->{'dispatch'}->{'GPSCoords'}->{'Latitude'} ),
        '%Longitude'        => encode_entities( $self->{'dispatch'}->{'GPSCoords'}->{'Longitude'} ),
        '%TypeCode'         => encode_entities( $self->{'dispatch'}->{'CallType'} ),
        '%CallNature'		=> encode_entities( $self->{'dispatch'}->{'CallLabel'} ),
        '%CallGroup'        => encode_entities( $self->{'dispatch'}->{'CallGroup'} ),
        '%UnitList'         => encode_entities( $self->{'dispatch'}->{'UnitDispatch'}->{'UnitList'} ),
        '%UnitAssign'       => $unitassg
    };

    my $s = join '|', keys %{ $regex };
    my $regex_s = qr/$s/;

    $led->{'title'} =~ s/($regex_s)/$regex->{$1}/g;
    $led->{'scroll'} =~ s/($regex_s)/$regex->{$1}/g;
    
    return $led;	
}
1;