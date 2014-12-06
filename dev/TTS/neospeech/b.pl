#!/usr/bin/perl

my $units = 'E828B E830 E848B E809C TK828 TW833 TW807';

my %replace = (
    'E'     => 'Engine ',
    'TW'    => 'Tower ',
    'TK'    => 'Truck ',
    'A'     => 'Ambulance ',
    'M'     => 'Medic ',
    'PA'    => 'Paramedic Ambulance ',
    'TN'    => 'Tanker ',
    'BFC'   => 'Battalion Chief ',
    'VFC'   => 'Volunteer Chief ',
    'DC'    => 'Duty Chief '
);

my $regex = join "|", keys %replace;
$regex = qr/$regex/;

$units =~ s/($regex)/$replace{$1}/g;
$units =~ s/([0-9]{3}[A-Z]{0,1})/<say-as interpret-as="characters" format="vxml:digits" detail="1 2 1">$1<\/say-as><break time="500ms"\/>/g;
print $units;
