#!/usr/bin/perl

my $box = '317';
my $search = '^([0-9])([0-9]{1,})$';
my $replace = 'sub{ my $zone=$1; $area=$2; "fire zone $zone <say-as interpret-as=\"vxml:".($area=~m/^0/?"digits":"number")."\">$area</say-as>';

my $regex_s = qr/$search/;
my $regex_r = eval $replace;
print "Box: $box\n";
$box =~ s/$regex_s/$regex_r/e;
print "Box: $box\n";
