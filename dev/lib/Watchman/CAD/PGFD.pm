package Watchman::CAD::PGFD;

$| = 1;

use base qw( Watchman::CAD );

my $self = {
	'call_alert'		=>	{},
	'incident_prefix'	=>	'PF'
};

sub fetch
{	
	my @inc;
	my $sts;
	
	$self->{'incident_prefix'} = 'PF';
	$self->{'ua'} = $_[0]->SUPER::ua;
	my $preIncAlert = $_[0]->SUPER::preIncAlert;
	
	my $request = $_[0]->SUPER::request;	
	my $content = "Inc=abc&hour=16&min=23";
	
	$request->content( $content );		
	
	my $response = $self->{'ua'}->request( $request );
	
	if ( $response->status_line eq '200 OK' ) {
	
		print "Request is good\n\n";
	
		my $content = $response->content;
		
		while ( $content =~ /(P[0-9]{9})\s+([0-9]{2}:[0-9]{2})\s+([0-9]{2}:[0-9]{2})?\s+[1-9]{1}\s([A-Z]{2,9})\s+([0-9]{4})?\s+(PF[0-9]{0,10})?\s{1,14}(.+?)<br>/g ) {

			if ( defined $preIncAlert->{ substr( $5, 0, 2 ) }->{ trim( $4 ) } || trim( $1 ) eq $self->{'call_alert'}->{'cno'} ) { # Filter by box area & call type, or if previous incident alert has been modified		
			
				if ( ( ! trim( $6 ) || ( trim( $6 ) && trim( $6 ) eq $self->{'incident_prefix'} ) ) && trim( $3 ) ) { # Cancelled if there is no inc no, or inc no is just the prefix with a close time
					$sts = 'CANCELLED';
				} elsif ( trim( $1 ) && ! trim( $3 ) && ( ! trim( $6 ) || ( trim( $6 ) && trim( $6 ) eq $self->{'incident_prefix'} ) ) ) { # Pending if call no and open time exist, close time does not exist and incident no is empty or prefix only
					$sts = 'PENDING';
				} elsif ( ( trim( $6 ) && trim( $6 ) ne $self->{'incident_prefix'} ) && trim( $2 ) ) { # Dispatched if incident no exists and is not incident prefix, open time exists and close time does not
					$sts = 'DISPATCHED';
				}
						
				push @inc, {
					'cno'   =>  trim( $1 ),
					'typ'   =>  trim( $4 ),
					'box'   =>  trim( $5 ),
					'inc'   =>  trim( $6 ),
					'sts'   =>  $sts,
					'loc'   =>  trim( $7 ),
				};
			}
		}		
	}
	
	foreach my $_i ( reverse @inc ) {
	
		if ( $_i->{'sts'} ne 'DISPATCHED' ) {
		
			$_i->{'loc'} =~ s/^(?:DWE|AL)\s?(.*)$/$1/ if ( $_i->{'loc'} =~ /^(?:DWE|AL)(.*)$/ );	
			print "$sts $_i->{cno} $_i->{loc} $_i->{box} $_i->{typ} $_i->{loc} \n";
			
			if ( $_i->{'sts'} eq 'CANCELLED' ) {
			
				print "Cancelled\n";
				
			} else {
			
				$self->{'call_alert'} = {
					'cno'      =>  $_i->{'cno'},
					'box'      =>  substr($_i->{'box'}, 0, 2),
					'fullBox'  =>  $_i->{'box'},
					'typ'      =>  $_i->{'typ'},
					'typName'  =>  $self->{'preIncType'}->{ $_i->{'typ'} }->{'typeName'},
					'loc'      =>  $_i->{'loc'},
				};							
			}					
		}
	}	
	
	return $self->{'call_alert'};
}

sub trim
{
    my $string = shift;
    if ( $string ) {
        $string =~ s/^\s+//;
        $string =~ s/\s+$//;
        return $string;
    }
}

1;