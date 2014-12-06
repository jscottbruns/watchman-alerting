#!/usr/bin/perl
use strict;
use POSIX;
use DBI;
use DBIx::Connector;
use Exception::Class::DBI;

DBI->trace(2, '/var/log/watchman-alerting/dbtrace.log');

my $DBH2;
unless (
    $DBH2 = DBIx::Connector->new(
        'dbi:mysql:Watchman_iCAD',
        'root',
        'ava457',
        {
            PrintError=> 0,
            RaiseError=> 0,
            HandleError=> Exception::Class::DBI->handler,
            AutoCommit=> 1,
        }
    )
)
{
    print "Can't connect to iCAD\n";
    exit;
}

my $DBH;
unless (
	$DBH = DBIx::Connector->new(
		'dbi:ODBC:CambriaDSN',
		'fa',
		'firehouseautomation',
		{
			PrintError=> 0,
        	RaiseError=> 0,
    	    HandleError=> Exception::Class::DBI->handler,
	        AutoCommit=> 1,
		}
	)
) {
	die "Database connection error: " . $DBI::errstr;
}

my $ref;
eval {
	$ref = $DBH->run( sub {
		return $_->selectall_arrayref(
			"SELECT RTRIM(TruckId) AS UnitId, RTRIM(Truck) AS Unit, RTRIM(Incident) AS IncidentNo, CONVERT(varchar, Dispatched, 126) AS Dispatch, CONVERT(varchar, Enroute, 126) AS Enroute, CONVERT(varchar, OnScene, 126) AS Onscene, CONVERT(varchar, Available, 126) AS InService, RTRIM(Status) FROM dbo.tblActiveTrucks WHERE Incident = ? AND ( Dispatched >= ? OR Enroute >= ? OR OnScene >= ? OR Available >= ? OR AvailOnScene >= ? )",
			{ Slice => {} },
			'201111-28798',
			( ('2011-11-23 14:43:30') x 5 )
		);
	} )
};

if ( my $ex = $@ )
{
    print "Error ", $ex->error, "\n";
}

foreach my $_i ( @{ $ref } ) {
	print "$_i->{UnitId} $_i->{IncidentNo} $_i->{Dispatch} $_i->{Enroute} $_i->{Onscene} $_i->{InService} $_i->{Status}\n";
}
exit;

eval {
	$DBH2->run( sub {
		$_->do(
			qq{
				INSERT INTO Watchman_iCAD.Incident
				VALUES 
				(
							?, # EventNo
							?, # IncidentNo
							CURRENT_TIMESTAMP(),
							?, # EntryTime
							?, # OpenTime
							?, # DispatchTime
							?, # EnrouteTime
							?, # OnsceneTime
							?, # CloseTime
							?, # Status
							?, # CallType
							?, # Nature
							?, # BoxArea
							?, # StationGrid
							?, # Location
							?, # LocationNote
							?, # CrossSt1
							?, # CrossSt2
							?, # GPSLatitude
							?, # GPSLongitude
							?, # Priority
							?, # RadioTac
							?  # MapGrid
				)
				ON DUPLICATE KEY UPDATE
							DispatchTime = ?,
							EnrouteTime = ?,
							OnsceneTime = ?,
							CloseTime = ?,
							Status = ?,
							CallType = ?,
							Nature = ?,
							BoxArea = ?,
							StationGrid = ?,
							Location = ?,
							LocationNote = ?,
							CrossSt1 = ?,
							CrossSt2 = ?,
							GPSLatitude = ?,
							GPSLongitude = ?,
							Priority = ?,
							RadioTac = ?,
							MapGrid = ?
			},
						undef,
						( defined $ref->{'EventNo'} ? $ref->{'EventNo'} : $ref->{'IncidentNo'} ),
						$ref->{'IncidentNo'},
						$ref->{'EntryTime'},
						( defined $ref->{'OpenTime'} ? $ref->{'OpenTime'} : undef ),
						( defined $ref->{'DispatchTime'} ? $ref->{'DispatchTime'} : undef ),
						( defined $ref->{'EnrouteTime'} ? $ref->{'EnrouteTime'} : undef ),
						( defined $ref->{'OnsceneTime'} ? $ref->{'OnsceneTime'} : undef ),
						( defined $ref->{'CloseTime'} ? $ref->{'CloseTime'} : undef ),
						( defined $ref->{'Status'} ? $ref->{'Status'} : undef ),
						$ref->{'CallType'},
						$ref->{'Nature'},
						$ref->{'BoxArea'},
						$ref->{'StationGrid'},
						$ref->{'Location'},
						$ref->{'LocationNote'},
						$ref->{'CrossSt1'},
						$ref->{'CrossSt2'},
						( defined $ref->{'GPSLatitude'} ? $ref->{'GPSLatitude'} : 0 ),
						( defined $ref->{'GPSLongitude'} ? $ref->{'GPSLongitude'} : 0 ),						
						$ref->{'Priority'},
						$ref->{'RadioTac'},
						$ref->{'MapGrid'},
						( defined $ref->{'DispatchTime'} ? $ref->{'DispatchTime'} : undef ),
						( defined $ref->{'EnrouteTime'} ? $ref->{'EnrouteTime'} : undef ),
						( defined $ref->{'OnsceneTime'} ? $ref->{'OnsceneTime'} : undef ),
						( defined $ref->{'CloseTime'} ? $ref->{'CloseTime'} : undef ),
						( defined $ref->{'Status'} ? $ref->{'Status'} : undef ),
						$ref->{'CallType'},
						$ref->{'Nature'},
						$ref->{'BoxArea'},
						$ref->{'StationGrid'},
						$ref->{'Location'},
						$ref->{'LocationNote'},
						$ref->{'CrossSt1'},
						$ref->{'CrossSt2'},
						( defined $ref->{'GPSLatitude'} ? $ref->{'GPSLatitude'} : 0 ),
						( defined $ref->{'GPSLongitude'} ? $ref->{'GPSLongitude'} : 0 ),
						$ref->{'Priority'},
						$ref->{'RadioTac'},
						$ref->{'MapGrid'}
		);
	} )
};

if ( my $ex = $@ )
{
	print "Insert error: " . $ex->error;
	exit;
}

print "Done\n";
exit;



