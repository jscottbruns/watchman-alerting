#!/usr/bin/perl
use DBI;
use POSIX;

my $dbh = DBI->connect(
	'dbi:mysql:Watchman_iCAD',
	'root',
	'ava457'
) or die "Can't connect";

my $i = $ARGV[0];
die "No arg specified\n" unless $i;

$file = 'icad-incident.sqldump' if $i == 1;
$file = 'icad-incident.sqldump1-a.sql' if $i == 2;
$file = 'icad-incident.sqldump2' if $i == 3;

if ( open(FH, "<$file") )
{
	while ( <FH> )
	{
		$sql .= $_;
	}
	close FH;
}
else
{
	print "Can't open file $file\n";
	exit;
}

my $date = POSIX::strftime('%Y-%m-%d %H:%M:%S', localtime);

while ( $sql =~ /^(.*)/mg )
{
	if ( $1 )
	{
		$s = $1;
		$s =~ s/(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})/$date/gm;

		print "Executing query [$s]\n";
		$dbh->do( $s ) or die "SQL error: " . $dbh->errstr . " \n  $s\n";
	}
}
print "Done\n";
