package Parse_PG;

sub new 
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};

    $self->{'data'}     =   $params{'data'};	
	
	bless $self, $class;
	
	return $self->parse_packet;
}

sub parse_packet 
{
	my $self = shift;
	
sub parseDispatch
{
    my $self = shift;

    my ($search_q, $searchAgainst);
    my @search;

    &main::write_log("Parsing run sheet\n");

    my $lookup = $self->{'elements'}->{'incidentNo'};
    if ( $self->{'packet'} =~ /($lookup)\s?(P[0-9]{9}).*/m ) {
        $self->{'incidentNo'} = $2;
    }
    my $lookup = $self->{'elements'}->{'location'};
    if ( $self->{'packet'} =~ /($lookup)\s+(.*)/m ) {
        $self->{'location'} = &main::trim( $2 );
    }
    my $lookup = $self->{'elements'}->{'area'};
    if ( $self->{'packet'} =~ /($lookup)\s+([0-9]{2})/m ) {
        $self->{'area'} = &main::trim( $2 );
    }
    my $lookup = $self->{'elements'}->{'type'};
    if ( $self->{'packet'} =~ /($lookup)\s+([A-Z]{2,9})/m ) {
        $self->{'callType'} = &main::trim( $2 );
    }
    my $lookup = $self->{'elements'}->{'comment'};
    if ( $self->{'packet'} =~ /($lookup)\s+(.*)/m ) {
        $self->{'comment'} = ( length( $2 ) > 45 ? substr( &main::trim( $2 ), 45) : &main::trim( $2 ) );
    }

    my $lookup = $self->{'elements'}->{'unitAssignment'};
    my $match;
    while ( $self->{'packet'} =~ /\s+($lookup)\s+(.+)/mg ) {
        $match = &main::trim( $2 );
        while ( $match =~ /([A-Z]{1,3}[0-9]{1,3})/g ) {
            $self->{'unitAssignment'} .= $1 . " ";
        }
    }

    return $self->NewIncident if ( $self->{'incidentNo'} && $self->{'location'}&& $self->{'area'} && $self->{'callType'} && $self->{'unitAssignment'} );

    return undef;
}	
}
1;