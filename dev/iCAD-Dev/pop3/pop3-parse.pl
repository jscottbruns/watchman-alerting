#!/usr/bin/perl
#
# Parse POP3 message body from specified file
#
use File::Random qw/:all/;
use DateTime;

my $file = $ARGV[0];
my $dir_root = "/usr/local/watchman-icad/";

if ( -d $dir_root . $file )
{
	print "Selecting random file from $file\n";
	$file .= random_file( -dir => $dir_root . $file );
} 

$file = $dir_root . $file;

die "Can't read file [$file]\n" unless -f $file;

print "Reading from file [$file]\n";

my ($body, $head);
if ( open(FH, "<$file") )
{
	while ( <FH> )
	{
    	$body .= $_;
	}
	close FH;
}
else
{
	die "Can't open file for reading [$file]\n";
}

die "No data read from file\n" unless $body;

my $month_names = {
	'Jan'	=> 1,
	'Feb'	=> 2,
	'Mar'	=> 3,
	'Apr'	=> 4,
	'May'	=> 5,
	'Jun'	=> 6,
	'Jul'	=> 7,
	'Aug'	=> 8,
	'Sep'	=> 9,
	'Oct'	=> 10,
	'Nov'	=> 11,
	'Dec'	=> 12
};

$body =~ s/(=\r\n)//mg;
if ( $body =~ /^(.*?)---------(.*)$/ms )
{
	$head = &trim($1);
    $body = &trim($2);
}

my ($IncTime, $IncDate);

$head =~ /^Date:\s+(.*)$/im and $IncDate = &trim($1);

if ( $IncDate =~ /^([a-zA-Z]{3}),\s+([0-9]{1,2})\s([A-Za-z]{3,4})\s([0-9]{4})\s([0-9]{2}):([0-9]{2}):([0-9]{2})\s([+\-0-9]{5})$/ )
{    
	$dt = DateTime->new(
		year       => $4,
	    month      => $month_names->{ $3 },
	    day        => $2,
	    hour       => $5,
	    minute     => $6,
	    second     => $7,
	    time_zone  => $8
	);
	
	$IncTime = $dt->epoch(); 		    
}

print "*ERROR* Failed to parse POP3 timestamp header - Unknown entry timestamp" unless ( $IncTime );

print "\n[$body]\n\n";
#$body = "F122900091: BLS Amb, HIGH BRIDGE RD/EB OLD ANNAPOLIS RD, PP,  <0/ 13400>, TGA2, 1815, Unit:A818";

$body =~ s/(=\r\n)//mg;
my ($XStreet1, $XStreet2);

if ( $body =~ /^(.*):\s+(.*?),\s+(.*?),(?:\s+([a-zA-Z]{2}),)?(?:\s+((?:at|btwn\s+)?.*?),)?\s+(T[ABCDEGL|1234567890]*),\s+([0-9A-Z]{2,}),\s+Unit(?:s)?:(.*)/ )
{
    $IncNo = &main::trim($1);
    $Nature = &main::trim($2);
    $Address = &main::trim($3);    
    $LocationInfo = &main::trim($5);
    $LocationDescr = undef;
    $LocationApt = undef;
    $XStreet1 = undef;
    $XStreet2 = undef;
    $Agency = &main::trim($4);
    $Tac = &main::trim($6);
    $BoxArea = &main::trim($7);
    $Units = &main::trim($8);

	if ( $Address =~ /^(.*?)\s#(.*)$/ )
	{
		$LocationApt = &trim( $2 );	
	} 
	
    if ( $LocationInfo =~ /^btwn\s+(.*?)\sand\s(.*)$/ )
    {
        $XStreet1 = &trim($1);
        $XStreet2 = &trim($2);
    }
    elsif ( $LocationInfo =~ /^at\s+(.*)$/ )
    {
    	$LocationDescr = &trim($1);
    	$LocationDescr = &trim($1) if ( $LocationDescr =~ /(^.*?)(,\s+$Agency)$/ );
    	
    	if ( $LocationDescr )
    	{
    		my $_tmp = $Address;
    		$Address = $LocationDescr;
    		$LocationDescr = $_tmp if $_tmp;
    	}
    }

	print "*************************** Parsed Incident Data ***************************
     IncidentNo: $IncNo
   IncidentTime: $IncTime
         Nature: $Nature      
  LocationDescr: $LocationDescr
LocationAddress: $Address
    LocationApt: $LocationApt
   LocationNote: $LocationInfo
   CrossStreet1: $XStreet1
   CrossStreet2: $XStreet2
         Agency: $Agency
            TAC: $Tac
        BoxArea: $BoxArea
        Station: $Station
          Units: $Units";
          
	print "\n\n";
	print `mysql -h icad-rds.pgmd.fhwm.net -u icad_pgmd --password=firehousepgmd -D ICAD_PGMD -e "SELECT t1.IncidentNo, t1.EntryTime AS IncidentTime, IFNULL(t1.Nature,'') AS Nature, IFNULL(t1.LocationDescr, '') AS LocationDescr,IFNULL(t1.LocationAddress, '') AS LocationAddress,IFNULL(t1.LocationApartment, '') AS LocationApt,IFNULL(t1.LocationNote, '') AS LocationNote,IFNULL(t3.CrossStreet1, '') AS CrossStreet1,IFNULL(t3.CrossStreet2, '') AS CrossStreet2,IFNULL(t1.Agency, '') AS Agency,IFNULL(t1.District, '') AS TAC,IFNULL(t1.BoxArea, '') AS BoxArea,IFNULL(t1.StationGrid, '') AS Station,GROUP_CONCAT( IFNULL(t2.Unit, '') SEPARATOR ' ' ) AS UnitList FROM Incident t1 LEFT JOIN IncidentUnit t2 ON t1.EventNo = t2.EventNo LEFT JOIN IncidentGeoInfo t3 ON t1.IncidentNo = t3.IncidentNo WHERE t1.IncidentNo = '$IncNo'"`;
	print "\n\n";
}
else
{
    print "Failed to match string regexp\n";
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
    