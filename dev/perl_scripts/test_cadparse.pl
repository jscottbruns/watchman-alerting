#!perl
use DBI;

my $content;
if ( open(FH, "<CAD_winpcap\\CAD_SingleIncResp.html") ) {

	while ( <FH> ) {
		$content .= $_;
	}

	close FH;
}

my $dbh = DBI->connect(
	"dbi:SQLite:dbname=..\\watchman.s3db",
	"",
	"",
	{
	   PrintError => 1
	}
);

sub write_log
{
	my $m = shift;
	print $m;
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

$self->{'regexp'} = {};

unless ( $sth = $dbh->prepare("SELECT *
							   FROM pollingRegex")
) {

	&main::write_log("Database query error when fetching [pollingRegex] values: ($DBI::err) $DBI::errstr \n", 1);
	exit;
}

if ( $sth->execute ) {

	$sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
	while ( $sth->fetch ) {

		$self->{'regexp'}->{ $row{'element'} } = $row{'regexp'};
	}
}

if ( $content =~ /$self->{'regexp'}->{'callNo'}/m ) {

	print "Parsing Single Incident Regexp\n";
	
	$_vars = {};
	$_vars->{'cno'} = trim( $1 );

	if ( $content =~ /$self->{'regexp'}->{'incidentStatus'}/ ) {

		$_vars->{'sts'} = 'CANCELLED' if ( trim( $1 ) =~ /$self->{'regexp'}->{'statusCancelled'}/ );
		$_vars->{'sts'} = 'PENDING' if ( trim( $1 ) =~ /$self->{'regexp'}->{'statusPending'}/ );
		$_vars->{'sts'} = 'DISPATCHED' if ( ! $_vars->{'sts'} );
	}

	$_vars->{'typ'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'incidentType'}/ );
	$_vars->{'box'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'area'}/ );
	$_vars->{'inc'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'incidentNo'}/ );
	$_vars->{'loc'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'location'}/ );
	$_vars->{'ope'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'opentime'}/ );
	$_vars->{'clo'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'closetime'}/ );

	$_vars->{'dat'} = trim( $1 ) if ( $content =~ /$self->{'regexp'}->{'date'}/ );
	if ( $_vars->{'dat'} =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{2})/ ) {
		$_vars->{'dat'} = "$3-$1-$2";
	}
	
	for my $_i ( keys %{ $_vars } ) {
		print "$_i => $_vars->{ $_i }\n";
	}

} elsif ( $content =~ /$self->{'regexp'}->{'pollingList'}/ ) {

	print "Parsing Polling Regexp\n";

	my $i = 1;
	while ( $content =~ /$self->{'regexp'}->{'pollingList'}/g ) {

		print "[$i] $1 $2 $3 $4 $5 $6 $7\n";
		$i++;
	}

}