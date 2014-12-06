#!/usr/bin/perl
use DateTime;
my $dt = DateTime->now();

my $units = $ref->{'UnitList'};
my %replace = (
    'E'     => 'Engine',
    'TW'    => 'Tower',,
    'TK'    => 'Truck',
    'A'     => 'Ambulance',
    'MD'     => 'Medic',
    'PM'     => 'Paramedic',
    'PA'    => 'Paramedic Ambulance',
    'TN'    => 'Tanker',
    'BO'   => 'Battalion Chief',
    'BFC'   => 'Battalion Chief',
    'BC'   => 'Battalion Chief',
    'VFC'   => 'Volunteer Chief',
    'VC'   => 'Volunteer Chief',
    'DC'    => 'Duty Chief',
    'DUTY'    => 'Duty Chief',
    'DTY'    => 'Duty Chief',
    #'AMD'	=> '',
    #'ATN'	=> '',
    #'BARI'	=> '',
    #'CPA'	=> '',
    #'CSQ'	=> '',
    #'HC'	=> '',
    #'HMD'	=> '',
    #'HSC'	=> '',
    #'HTN'	=> '',
    'MP'	=> 'Mini Pumper',
    #'NEMSDO'	=> '',
    #'NMR'	=> '',
    'PE'	=> 'Paramedic Engine',
    'RE'	=> 'Rescue Engine',
    #'SEMSDO'	=> '', 
    'SQ'	=> 'Rescue Squad',
    'RS'	=> 'Rescue Squad',
    'RES'	=> 'Rescue',
    #'WI'	=> '',
    'WS'	=> 'Water Supply',
    #'XE'	=> ''					    
);			

$units = 'E828B MD825';

my $regex = join "|", keys %replace;
$regex = qr/($regex)/;

my @u = split ' ', $units;
my @unitlist;
foreach  ( @u )
{
    $_unit = $_;

    $_unit =~ s/($regex)/$replace{$1} /g;
    $_unit =~ s/^(.*?)\s(.*)$/$1<say-as interpret-as="characters" format="vxml:digits" detail="1 2 1">$2<\/say-as><break time="500ms"\/>/;
    push @unitlist, $_unit;
}

my $tts_speak = "<s>". ( join '', @unitlist ) . "</s>";
print $tts_speak;
						
