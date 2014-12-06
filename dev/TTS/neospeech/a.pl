#!/usr/bin/perl

my $apt = '505';
my $addr = "12350-12360 HAMILTON ST ";
my $addr = "12350 MD RT 202";
my $addr = "PA TPK NEAR EXT 24";
#my $addr = "HAMILTON PY/BRADDOCK AV #505";

if ( $addr =~ /^([0-9\-]*)?\s?(.*)$/ )
{
    $AddressNum = $1;
    $AddressStreet = $2;
}

if ( ! $AddressStreet )
{
    $AddressStreet = $addr;
}

if ( $AddressStreet =~ /^(.*?)(#$apt)?$/ )
{
    $AddressStreet = $1;
    $AddressApt = $2;
}

my ($intersect, $Street1, $Street2);

$AddressStreet =~ s/\b(AV|AVE)\b/Avenue/gi;
$AddressStreet =~ s/\b(BL|BLV|BLVD)\b/Boulevard/gi;
$AddressStreet =~ s/\b(CSW|CSWY)\b/Causeway/gi;
$AddressStreet =~ s/\b(CNT|CTR)\b/Center/gi;
$AddressStreet =~ s/\b(CI|CIR|CR)\b/Circle/gi;
$AddressStreet =~ s/\b(CT)\b/Court/gi;
$AddressStreet =~ s/\b(DR)\b/Drive/gi;
$AddressStreet =~ s/\b(EX|EXP|EXPR|EXPY)\b/Expressway/gi;
$AddressStreet =~ s/\b(EXT)\b/Extension/gi;
$AddressStreet =~ s/\b(FY|FWY|FRW)\b/Freeway/gi;
$AddressStreet =~ s/\b(GT|GTW|GTWY)\b/Gateway/gi;
$AddressStreet =~ s/\b(HTS)\b/Heights/gi;
$AddressStreet =~ s/\b(HW|HWY|HY)\b/Highway/gi;
$AddressStreet =~ s/\b(JCT|JT|JC)\b/Junction/gi;
$AddressStreet =~ s/\b(LNDG|LND|LD)\b/Landing/gi;
$AddressStreet =~ s/\b(LN|LA)\b/Lane/gi;
$AddressStreet =~ s/\b(MNR)\b/Manor/gi;
$AddressStreet =~ s/\b(MDWS)\b/Meadows/gi;
$AddressStreet =~ s/\b(MT|MNT)\b/Mount/gi;                  
$AddressStreet =~ s/\b(PKY|PY)\b/Parkway/gi;
$AddressStreet =~ s/\b(PK)\b/Pike/gi;
$AddressStreet =~ s/\b(PL)\b/Place/gi;
$AddressStreet =~ s/\b(PLZ|PZ)\b/Plaza/gi;
$AddressStreet =~ s/\b(PT)\b/Point/gi;
$AddressStreet =~ s/\b(PRT)\b/Port/gi;

$AddressStreet =~ s/\b(RT|RTE|ROU)\b/Route/gi;
$AddressStreet =~ s/\b(RPDS|RP|RPD)\b/Rapids/gi;
$AddressStreet =~ s/\b(RG|RDG)\b/Ridge/gi;
$AddressStreet =~ s/\b(RI|RIV|RV)\b/River/gi;
$AddressStreet =~ s/\b(RD)\b/Road/gi;
$AddressStreet =~ s/\b(SHR|SH)\b/Shore/gi;
$AddressStreet =~ s/\b(SHRS|SHS)\b/Shores/gi;

$AddressStreet =~ s/\b(SPG)\b/Spring/gi;
$AddressStreet =~ s/\b(SP|SPU|SPR)\b/Spur/gi;
$AddressStreet =~ s/\b(SQ|SQR)\b/Square/gi;
$AddressStreet =~ s/\b(STA)\b/Station/gi;
$AddressStreet =~ s/\b(ST|STR)\b/Street/gi;
$AddressStreet =~ s/\b(SMT|SM|SU)\b/Summit/gi;                  

$AddressStreet =~ s/\b(TER|TE)\b/Terrace/gi;
$AddressStreet =~ s/\b(TRL|TR)\b/Trail/gi;
$AddressStreet =~ s/\b(TUN|TL)\b/Tunnel/gi;
$AddressStreet =~ s/\b(TPKE|TPK|TK|TP|TRNPK)\b/Turnpike/gi;
$AddressStreet =~ s/\b(UN)\b/Union/gi;
$AddressStreet =~ s/\b(VLY|VAL)\b/Valley/gi;

$AddressStreet =~ s/\b(VIA|VDT)\b/Viaduct/gi;
$AddressStreet =~ s/\b(VW)\b/View/gi;
$AddressStreet =~ s/\b(VLG|VG)\b/Village/gi;
$AddressStreet =~ s/\b(VL|VIL)\b/Ville/gi;
$AddressStreet =~ s/\b(VIS|VS|VST)\b/Vista/gi;  
$AddressStreet =~ s/\b(WLK|WK|WL)\b/Walk/gi;
$AddressStreet =~ s/\b(WY)\b/Way/gi;

$AddressStreet =~ s/\b(PA)\b/Pennsylvania/gi;
$AddressStreet =~ s/\b(MD)\b/Maryland/gi;
$AddressStreet =~ s/\b(DE)\b/Deleware/gi;
$AddressStreet =~ s/\b(WV)\b/West Virginia/gi;
$AddressStreet =~ s/\b(VA)\b/Virginia/gi;
$AddressStreet =~ s/\b(OH)\b/Ohio/gi;

if ( $AddressStreet =~ /^(.*)\/(.*)$/ )
{
    $intersect = 1;
    $Street1 = $1;
    $Street2 = $2;
}

print "Numbers: $AddressNum\n";
if ( $intersect )
{
    print "Intersection of $Street1 and $Street2\n";    
}
else
{
    print "Street: $AddressStreet\n";
}
print "Apt: $AddressApt\n";
