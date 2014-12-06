#!/usr/bin/perl
use Getopt::Long;

my ( $file, $dest );
my $db = '-58d';
my $dur = 1;
my $thresh = 1.5;

my $result = GetOptions(
	'file=s'	=> \$file,
	'dest=s'	=> \$dest,
	'db=s'		=> \$db,
	'dur=s'		=> \$dur,
	'thresh=s'	=> \$thresh
);

die "Can't read source file [$file]\n" unless -f $file;
$dest = $file unless defined $dest;

print "sox $file $dest silence 0 $dur $thresh $db\n";
system( "sox $file $dest silence 0 $dur $thresh $db" );
