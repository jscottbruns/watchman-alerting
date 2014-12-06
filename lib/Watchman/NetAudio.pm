package Watchman::NetAudio;

use strict;
use warnings;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}



sub new {
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;

    &main::log("Starting Watchman::NetAudio service");

    my $self = {
    	'path'			=> $::PATH,
    	'debug'			=> $::DEBUG,
    	'temp_dir'		=> $::TEMP_DIR,
    	'dbh'			=> $::DBH,
    	'main'			=> $::self,
    	'log_dir'		=> $::LOG_DIR,
    	'audiobin'		=> '/usr/bin/perl /usr/local/watchman-alerting/bin/activate_zone',
    	'gstbin'		=> '/usr/bin/gst-launch-0.10',
    	'AlertGroups'	=> [],
    	'ZoneMembers'	=> [],
    };

	$self->{'settings'} = $::self->{'audioalerting'};
	$self->{'settings'}->{'PriorityPort'} = $self->{'settings'}->{'PriorityPort'} || '4446';
	$self->{'settings'}->{'NetAudioTTL'} = $self->{'settings'}->{'NetAudioTTL'} || '3';

    my $inc_type = $params{'IncType'}; # active(pending)|assigned(dispatched)|rftone(tone activation)
	my $units = $params{'Units'}; # array containing UnitID list for assigned incidents only
	my $audiosrc = $params{'AudioSrc'}; # array containing ordered list of audio to be distributed
	my $box_area = $params{'BoxArea'}; # Box area (string) to be compared against active incident box area
	my $alert_class = $params{'AlertClass'}; # For active incidents - pending|firstdue
	my $priority = $params{'Priority'}; # Priority incident w/specific priority preamble assigned
	my $tone_id = $params{'ToneID'};
	my $debug_flag = "-q";

	$priority = undef if $priority && ! -f File::Spec->catfile( $self->{'path'}, $audiosrc->[0] );

	if ( ! @{ $audiosrc } )
	{
		&main::log("No audio source parameters provided, nothing to do", E_ERROR);
		return undef;
	}

	my ($sql, $__sql);
	if ( $inc_type eq 'assigned' && @{ $units } )
	{
		$sql .= <<SQL;
OR
(
	t1.GroupType = 1
	AND
	t1.UnitID IN ( @{[ join(', ', map { "'$_'" } @{ $units } ) ]} )
)
SQL
	}
	elsif ( $inc_type eq 'active' )
	{
		$__sql = "t1.AlertQueue_LocalAlert = 1 " if $alert_class eq 'firstdue';
		$__sql = "t1.AlertQueue_PriAlert = 1 " if $alert_class eq 'pending';

		$sql .= <<SQL;
OR
(
	$__sql
	AND
	(
		t1.AlertQueue_Filter IS NULL OR '$box_area' REGEXP t1.AlertQueue_Filter
	)
)
SQL
	}
	elsif ( $inc_type eq 'rftone' )
	{
		$sql .= <<SQL;
OR
(
	t1.GroupType = 1
	AND
	(
		t1.ToneID IS NULL OR '$tone_id' REGEXP t1.ToneID 
	) 
)
SQL
	}

	eval {
		$self->{'AlertGroups'} = $self->{'dbh'}->run( sub {
			return $_->selectall_arrayref(
				qq{
					SELECT
						t1.GroupName,
						t1.GroupAddr,
						t1.GroupType,
						t1.AudioPreamble
					FROM AlertGroups t1
					WHERE t1.GroupType = 0 $sql
					GROUP BY t1.GroupAddr
				},
				{ Slice => {} }
			);
		} )
	};

	if ( my $ex = $@ )
	{
		&main::log("Database exception received while looking up NetAudio groups for audio delivery - Unable to proceed with audible alerting " . $ex->error, E_ERROR);
		return undef;
	}

	unless ( @{ $self->{AlertGroups} } )
	{
		&main::log("No audio groups found requiring audible alerting - Nothing to do at this time", E_ERROR);
		return undef;
	}

    &main::log("Found " . ( $#{ $self->{AlertGroups} } + 1 ) . " alert group(s) requiring audible alerting - Initiating audio delivery for " . ( $#{ $audiosrc } + 1 ) . " audio sources");

	my $addr_list = [];
	$debug_flag = "" if $self->{'debug'};

	foreach my $_i ( @{ $self->{AlertGroups} } )
	{
		push @{ $addr_list }, "'$_i->{GroupAddr}'";

		if ( ! ( fork ) )
		{
			&main::log("Preparing audio alerting for group " . ( $_i->{'GroupType'} == 1 ? '[UNIT]' : '[DEFAULT]' ) . " '$_i->{GroupName}' ($_i->{GroupAddr})");

			my ($audiolist, $_rtpcmd, $filetype);
			my $_jcount = 0;

			for my $_j ( @{ $audiosrc } )
			{
				my $__afile = $_j;

				if ( $_jcount == 0 && $inc_type eq 'assigned' && $_i->{'AudioPreamble'} && -f File::Spec->catfile( $self->{'path'}, $_i->{'AudioPreamble'} ) && ! $priority )
				{
					&main::log("Custom audio preamble assigned to audio group, replacing default audio preameble => [$_i->{AudioPreamble}]");
					$__afile = $_i->{'AudioPreamble'};
				}
				elsif ( $_i->{'GroupType'} == 1 )
				{
					my ($_fname, $_fext) = $__afile =~ /^(.*)\.([^.]+)$/;
					if ( $_fname && $_fext )
					{
						my $rampaudio = $_fname . "_rampup";

						if ( -f File::Spec->catfile( $self->{'path'}, $rampaudio . '.' . $_fext ) )
						{
							$__afile = $rampaudio . '.' . $_fext;
							&main::log("Ramp-up audio source identified ($__afile), replacing audio stream for alert group");
						}
					}
				}

				$_jcount++;

				$__afile = File::Spec->catfile( $self->{'path'}, $__afile) unless -f $__afile;
				$__afile =~ /\.([a-zA-Z0-9]{3})$/ and $filetype = $1;

				my $cmd;

		        if ( $filetype eq 'wav' )
		        {
		            $cmd = "$self->{gstbin} $debug_flag filesrc location=$__afile ! wavparse ! audioconvert ! audioresample ! mulawenc ! rtppcmupay ! udpsink host=$_i->{GroupAddr} auto-multicast=true port=$self->{settings}->{PriorityPort}";
		        }
		        elsif ( $filetype eq 'mp3' )
		        {
		            $cmd = "$self->{gstbin} $debug_flag filesrc location=$__afile ! mad ! audioconvert ! audioresample ! mulawenc ! rtppcmupay ! udpsink host=$_i->{GroupAddr} auto-multicast=true port=$self->{settings}->{PriorityPort}";
		        }
		        else
		        {
		        	&main::log("Unsupported file type [$filetype] found in audio file => [$__afile]", E_ERROR);
		        }

				qx{ $cmd };

				&main::log("$__afile ($?) => $_i->{GroupName} ($_i->{GroupAddr}) " . ( $self->{'debug'} ? "[$cmd]" : '' ));

				select(undef, undef, undef, 0.5);
			}

			&main::log("Finished processing audible alerting for group $_i->{GroupName} ($_i->{GroupAddr})");

			exit( 0 );
		}
	}

	return;
}

1;