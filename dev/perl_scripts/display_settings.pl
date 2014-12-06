#!/usr/bin/perl
use strict;
use DBI;

my $dbh = DBI->connect(
	'dbi:mysql:WatchmanAlerting',
	'root',
	'ava457',
	{
		PrintError	=> 0,
		RaiseError	=> 1
	}
) or die "Can't connect: $DBI::errstr\n";

my $s = $dbh->selectall_arrayref( 
	q{
		SELECT *
		FROM Settings
	},
	{ Slice => {} }
) or die "Error running query\n";

my $self;
foreach my $_i ( @$s )
{
	$self->{ $_i->{Category} }->{ $_i->{Name} } = $_i->{Setting};
}

for my $_i ( keys %{ $self } ) 
{
	print "$_i => $self->{ $_i } \n";
	for my $_j ( keys %{ $self->{ $_i } } )
	{
		print "  $_j => $self->{ $_i }->{ $_j } \n";
	}
	print "\n";
}
