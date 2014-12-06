package Watchman::HostSync;

use strict;
use SOAP::Lite;

sub new
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};

    &hostsync_log("Starting Watchman::HostSync - Incident Syncronization Service\n");

    $self->{'version'}          =   $::VERSION;
    $self->{'path'}             =   $::PATH;
    $self->{'debug'}            =   $::DEBUG;
    $self->{'license'}          =   $::LICENSE;

    $self->{'uri'}      =   $params{'uri'};
    $self->{'proxy'}    =   $params{'proxy'};
    $self->{'timeout'}  =   $params{'timeout'};
    $self->{'user'}     =   $params{'user'};
    $self->{'pass'}     =   $params{'pass'};
    my $inc             =   $params{'inc'};

    unless (
	    $self->{'dbh'} = DBI->connect(
	        "DBI:mysql:watchman",
	        $self->{'user'},
	        $self->{'pass'},
	        {
	           PrintError => 0
	        }
    ) ) {

        &hostsync_log("Database connection error: " . ( $DBI::errstr ? $DBI::errstr : $! ) . "\n", 1);
        return undef;
    }

    $self->{'hostsync_update'} = undef;
    $self->{'hostsync_cache'} = [];

	bless $self, $class;

    foreach my $_i ( reverse @{ $inc } ) {

        if ( my $sth = $self->{'dbh'}->prepare("SELECT
                                                    t1.incident_no,
                                                    t1.closetime,
                                                    t1.calltype,
                                                    t1.box,
                                                    t1.location
                                                FROM incidentpoll t1
                                                WHERE t1.call_no = ?")
        ) {

            unless ( $sth->execute( $_i->{'cno'} ) ) {

                &hostsync_log("Database execute error: " . $DBI::errstr . "\n", 1);
                next;
            }

            my $ref = $sth->fetchrow_hashref();

            if ( ! $ref || ( $ref && ( ( $_i->{'inc'} && ! $$ref{'incident_no'} ) || ( $_i->{'clo'} && ! $$ref{'closetime'} ) || $$ref{'calltype'} ne $_i->{'typ'} || $$ref{'box'} ne $_i->{'box'} || $$ref{'location'} ne $_i->{'loc'} ) ) ) {

                &hostsync_log( ( $ref ? "Incident Update - " : "New Incident - " ) . "Writing call #" . $_i->{'cno'} . " to database\n");

                if ( $self->{'dbh'}->prepare("REPLACE INTO incidentpoll
                                              VALUES
                                              (
                                                  ?,
                                                  ?,
                                                  CURDATE(),
                                                  ?,
                                                  ?,
                                                  ?,
                                                  ?,
                                                  ?
                                              )")->execute(
                                                  $_i->{'cno'},
                                                  ( $_i->{'inc'} ? $_i->{'inc'} : undef ),
                                                  $_i->{'ope'},
                                                  ( $_i->{'clo'} ? $_i->{'clo'} : undef ),
                                                  $_i->{'typ'},
                                                  $_i->{'box'},
                                                  $_i->{'loc'},
                                              )
                ) {

                    $self->__cache(
                        'cno'   =>  $_i->{'cno'},
                        'inc'   =>  $_i->{'inc'},
                        'ope'   =>  $_i->{'ope'},
                        'clo'   =>  $_i->{'clo'},
                        'typ'   =>  $_i->{'typ'},
                        'box'   =>  $_i->{'box'},
                        'loc'   =>  $_i->{'loc'}
                    );

                } elsif ( $DBI::errstr ) {

                	&hostsync_log("Database prepare error: $DBI::errstr \n", 1);
                }
            }
        }
    }

    if ( defined $self->{'hostsync_update'} ) {

        return $self->hostsync;
    }

    return -1;
}

sub __cache
{
    my $self = shift;
    my %params = @_;

    $self->{'hostsync_update'} = 1;

    push @{ $self->{'hostsync_cache'} },
    {
        %params
    };
}

sub hostsync
{
	my $self = shift;

	&hostsync_log("Initiating HTTP hostsync service to $self->{uri}/$self->{proxy} \n");

	my $response;
	unless (
		$response = SOAP::Lite->uri(
			$self->{'uri'}
		)->proxy(
			$self->{'proxy'},
			'timeout' => $self->{'timeout'}
		)->init( {
			'service'	=>	'hostsync',
			'license'	=>	$self->{'license'},
			'incident'	=>  $self->{'hostsync_cache'}
		} )->result
	) {

		&hostsync_log("SOAP server error: $response \n", 1);
		return undef;
	}

	return 1;
}

sub hostsync_log
{
    my $msg = shift;
    my $err = shift;
    my ($package, $file, $line) = caller;

    $msg =~ s/\n$//;
    $msg .= " (" . $$ . ")" if substr($$, 0, 1) eq '-';
    $msg = "[" . Date::Format::time2str('%C', time) . "] [" . $package . ":" . $line . "] " . ($err ? "[error] " : undef) . " $msg\n";

    if ( open(FHLOG, ">>$::PATH\\hostsync.log") ) {

        print FHLOG $msg;
        close FHLOG;
    }
}

1;