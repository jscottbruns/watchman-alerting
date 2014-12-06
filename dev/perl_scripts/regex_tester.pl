#!/usr/bin/perl

#my $box = '213-31';
#my $s = 4;
#my $e = length $box;

#my $newbox = substr( $box, $s, $e);

#my $format = '^([0-9])()$';

#print $newbox;exit;

my $box = $ARGV[0];
die "No box" unless $box;

my $reg = '([1]{1}([0-9*]))|([2]{1}([0-9]*))|([3]{1}([0-9*]))|([4]{1}([0-9*]))';
my $r = qr/$reg/;

my $start = 4;
my $end;

$end = length $box unless defined $end;

print "Regex: [$reg]\n";
print "Match\n" if $box =~ m/^($r)$/;

exit;

