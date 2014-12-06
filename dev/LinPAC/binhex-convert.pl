#!/usr/bin/perl
use Data::Translate;
use Data::Dumper;

my $d = new Translate;

my @hh = qw(cc);
my ($s, @ha) = $d->h2b( @hh );
print "[H2B:C] ". join('', @ha), "\n";

my @arr = qw(11001100);
my ($s, @ha) = $d->b2h(@arr);
print "[B2H] (" . @ha . ") ". join('', @ha), "\n";
