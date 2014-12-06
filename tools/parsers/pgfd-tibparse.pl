#!/usr/bin/perl
use XML::Generator;
use Getopt::Long;

use strict;

$| = 1;

my $verbose;
my $sourcefile;
my $destfile;
my $myunits;
my $input;

my $result = GetOptions(
	"source=s"	=> \$sourcefile,
	"dest=s"	=> \$destfile,
	"units=s"	=> \$myunits,
	"verbose"	=> \$verbose
);

if ( $sourcefile )
{
	if ( -f $sourcefile && open(FH, "<$sourcefile") )
	{
		while ( <FH> )
		{
			$input .= $_;
		}
		close FH;
	}
	else
	{
		die "Can't read from file [$sourcefile]\n";
	}
}
else
{
	while ( <> )
	{
		$input .= $_;
	}
}

die "Can't read input source\n" unless $input;

my $buff = {};
my $station_units = [ split( ' ', $myunits ) ] if defined $myunits;

( $buff->{'EventNo'} ) = ( $input =~ /Call #([A-Z0-9]*)\s/ );
( $buff->{'CallType'}, $buff->{'Nature'}, $buff->{'Priority'} ) = ( $input =~ /^Type:([A-Z0-9]*)\s-\s(.*)\sPriority:(.*)\s/m );
if ( $input =~ /As of ([\d]{2}\/[\d]{2}\/[\d]{4})\s([\d]{2}\:[\d]{2}:[\d]{2})/ )
{
	my $DispDate = trim($1);
	my $DispTime = trim($2);

	my ( $_m, $_d, $_y ) = ( $DispDate =~ /^([\d]{2})\/([\d]{2})\/([\d]{4})$/ );

	$buff->{'DispTime'} = $DispTime;
	$buff->{'DispDate'} = "$_y-$_m-$_d $DispTime";
}
if ( $input =~ /^Location:(.*?)\s?btwn\s(.*?)LocDesc:/sm )
{
	chomp( $buff->{'Location'} = &trim( $1 ) );

	chomp( my $cross = $2 );
	$cross =~ s/\n/\s/g;
	$cross = &trim( $cross );

	if ( $cross =~ /^(.*?)\sand\s(.*)/ )
	{
		$buff->{'CrossSt1'} = trim($1);
		$buff->{'CrossSt2'} = trim($2);

		$buff->{'CrossSt1'} =~ s/\n//g;
		$buff->{'CrossSt2'} =~ s/\n//g;
	}
}

( $buff->{'LocationNote'} ) = &trim( $input =~ /LocDesc:(.*)/ );
( $buff->{'MapGrid'} ) = &trim( $input =~ /Map:(.*)/ );
( $buff->{'CallTypeFinal'}, $buff->{'NatureFinal'} ) = ( $input =~ /Final Type:([A-Z0-9]*)\s-\s(.*)/ );
( $buff->{'RadioTac'}, $buff->{'Station'}, $buff->{'BoxArea'} ) = ( $input =~ /Darea:(.*)\sStation:(.*)\sBox:(.*)/ );
( $buff->{'IncidentNo'} ) = ( $input =~ /TIBNUM.*?Number:([A-Z0-9]*)/ );

my $ns = {
    ns1	=> [ ns1 => "http://niem.gov/niem/structures/2.0" ],
    ns2 => [ ns2 => "http://niem.gov/niem/domains/emergencyManagement/2.0" ],
    ns3 => [ ns3 => "http://niem.gov/niem/niem-core/2.0" ],
	ns4 => [ ns4 => "http://fhwm.net/xsd/ICadDispatch" ],
	ns6 => [ ns6 => "http://niem.gov/niem/domains/jxdm/4.0" ],
	ns7 => [ ns7 => "http://niem.gov/niem/aStreetNamensi-nist/2.0" ]
};

my $xml = XML::Generator->new( pretty => 1 );

my ($_time, $_type, $_fdid, $_txt, $_geo);
my $Meta_DispSvc = 'PRINTSTREAM';
my $Meta_Jurisd = "PRINCE GEORGE'S COUNTY";
my $Meta_TransID = '';
my $Meta_TransTime = '';

$buff->{'CurrentStatus'} = 'DISPATCHED';
$buff->{'UnitList'} = [];
$buff->{'UnitAssign'} = [];
$buff->{'UnitPreempt'} = [];
$buff->{'UnitReplace'} = [];
$buff->{'UnitBackup'} = [];

while ( $input =~ /^([\d]{2}:[\d]{2}:[\d]{2})\s*([a-z0-9\-]*)\s*\((.*?)\)\s*(.*?(?=[\d]{2}:[\d]{2}:[\d]{2}|\z))/msig )
{
	$_time = &trim($1);
	$_type = &trim($2);
	$_fdid = &trim($3);

	$_txt = &trim($4);
	$_txt =~ s/\s{2,}/ /g;

	$buff->{'EntryTime'} = $_time if ( $_type eq 'CREATE' && ! $buff->{'EntryTime'} );
	$buff->{'OpenTime'} = $_time if ( $_type eq 'ENTRY' && ! $buff->{'OpenTime'} );
	$buff->{'DispTime'} = $_time if ( $_type eq 'DISP' && ! $buff->{'DispTime'} );

	if ( $_type eq 'DISP' || $_type eq 'BACKUP' )
	{
		if ( $_txt =~ /^([a-zA-Z0-9]*)/ )
		{
			my $_unit = $1;

			push @{ $buff->{UnitList} }, $_unit if ( $_type eq 'DISP' || $_type eq 'BACKUP' );
			push @{ $buff->{UnitAssign} }, $_unit if ( ( $_type eq 'DISP' || $_type eq 'BACKUP' ) && &indexArray( $_unit, @{ $station_units } ) >= 0 && $_time eq $buff->{'DispTime'} );
		}
	}

	$_txt =~ s/\n//g;

	if ( $_type eq 'ALIGEO' && ! $buff->{'GPSLatitude'} && ! $buff->{'GPSLongitude'} )
	{
		( $buff->{'GPSLongitude'} ) = ( $_txt =~ /GeoLong:([-+]?[0-9]*\.?[0-9]+)/ );
		( $buff->{'GPSLatitude'} ) = ( $_txt =~ /GeoLat:([-+]?[0-9]*\.?[0-9]+)/ );
	}

	$buff->{'Narrative'} .= $xml->Comment(
		$xml->EntryTime( $_time ),
		$xml->EntryFDID( $_fdid ),
		$xml->EntryOperator,
		$xml->EntryID( $_type ),
		$xml->Text( $_txt )
	);
}

my $AssignedXML;
foreach my $_unit ( @{ $buff->{UnitAssign} } )
{
	$AssignedXML .= $xml->Assigned( $_unit );
}

my $xmlout = $xml->WatchmanAlerting(
	[
		fhwm	=> "http://fhwm.net/xsd/1.2/ICadDispatch",
		@{ $ns->{ns1} },
		@{ $ns->{ns2} },
		@{ $ns->{ns3} },
		@{ $ns->{ns4} },
		@{ $ns->{ns6} },
		@{ $ns->{ns7} },
	],
	$xml->IncidentNo( $buff->{IncidentNo} ),
	$xml->EventNo( $buff->{EventNo} ),
	$xml->Priority( $buff->{Priority} ),
	$xml->RadioTac( $buff->{RadioTac} ),
	$xml->DispatchDate( $buff->{DispDate} ),
	$xml->IncidentStatus(
		$xml->CurrentStatus(
			$buff->{CurrentStatus}
		),
		$xml->Entry( $buff->{'EntryTime'} ),
		$xml->Open( $buff->{'OpenTime'} ),
		$xml->Dispatch( $buff->{'DispTime'} ),
	),
	$xml->CallType(
		$xml->Nature( $buff->{Nature} ),
		$xml->TypeCode( $buff->{CallType} )
	),
	$xml->IncidentLocation(
		$xml->BoxArea( $buff->{BoxArea} ),
		$xml->Station( $buff->{Station} ),
		$xml->StationGrid( $buff->{BoxArea} ),
		$xml->MapGrid( $buff->{MapGrid} ),
		$xml->Location(
			$xml->StreetName( $buff->{Location} ),
			$xml->StreetName( $buff->{LocationNote} )
		),
		$xml->CrossStreet(
			$xml->StreetName( $buff->{CrossSt1} ),
			$xml->StreetName( $buff->{CrossSt2} ),
		),
		$xml->GPSCoords(
			$xml->GPSLatitude( $buff->{GPSLatitude} ),
			$xml->GPSLongitude( $buff->{GPSLongitude} ),
		)
	),
	$xml->UnitDispatch(
		$xml->UnitList( join( ' ', @{ $buff->{UnitList} } ) ),
		$AssignedXML
	),
	$xml->IncidentComments(	$buff->{'Narrative'} ),
	$xml->Metadata(
		$xml->DispatchService( $Meta_DispSvc ),
		$xml->Jurisdiction( $Meta_Jurisd ),
		$xml->TransID( $Meta_TransID ),
		$xml->TransTimestamp( $Meta_TransTime ),
	)
);

if ( $destfile )
{
	if ( open(FH, ">$destfile") )
	{
		print FH $xmlout;
		close FH;

		print "WatchmanAlerting incident data written to $destfile\n";
		exit 0;
	}
	else
	{
		print "Error - Can't write to destination file $destfile\n";
		exit 73;
	}
}
else
{
	print STDOUT $xmlout;
	exit 0;
}


sub rtrim($)
{
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}

sub ltrim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	return $string;
}

sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

sub indexArray
{
    1 while $_[0] ne pop;
    @_-1;
}