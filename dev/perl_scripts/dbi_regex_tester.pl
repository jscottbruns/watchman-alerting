#!/usr/bin/perl
use DBI;

my $h = &func;
print "A: $h->{a}\n";
print "B: $h->{b}\n";
exit;
sub func
{
    return {
        'a' => 1,
        'b' => 2
    };
}


my $dbh = DBI->connect('DBI:mysql:ICAD_MASTER', 'root', '') or die "Can't connect: $DBI::errstr";

#my $UnitListFormatted = "EN828 EN830 EN809 EN813B TK828 TW833 RS801 BO802";
my $UnitList = 'A814B E828B';
my $nature = 'E-1 FIRST RESPONDER DETAIL';
my $location = 'INWOOD ST';
my $xstreet1 = 'Inwood St';
my $xstreet2 = 'Frankstown Ave';
my $box = '307';

my $regex = [];
my $regex_rules = {};

my $regex = $dbh->selectall_arrayref(
    "SELECT t1.Category, t1.SearchKey, t1.ReplaceKey FROM FormattingRules t1 ",
    { Slice => {} }
);

foreach my $_reg ( @$regex )
{               
    $regex_rules->{ $_reg->{'Category'} } = [] unless defined @{ $regex_rules->{ $_reg->{'Category'} } };
                    
    push @{ $regex_rules->{ $_reg->{Category} } }, {
        'search'    => $_reg->{'SearchKey'},
        'replace'   => $_reg->{'ReplaceKey'}
    };                          
}

my @units = split ' ', $UnitList;
my $numunits = 0;               

print "Nature: " . &formatTextString( $nature, 'nature' ) . "\n";
print "Location: " . &formatTextString( $location, 'location') . "\n";
print "Xstreet1: " . &formatTextString( $xstreet1, 'location') . "\n";
print "XStreet2: " . &formatTextString( $xstreet2, 'location') . "\n";
print "Box: " . &formatTextString( $box, 'box') . "\n";

foreach ( @units )
{
    $numunits++;
    print "Checking unit [$_]\n";
    $_ = &formatTextString( $_, 'unit' );
}                                                                               

print "UnitList: ";
print join ' ', @units;
print "\n";exit;
if ( $numunits > 0 )
{                                                                   
    $tts_speak .= "<s>";
    $tts_speak .= join '<break/>', @units;
    $tts_speak .= ( $numunits > 1 ? ' have ' : ' has ' ) . 'been alerted';
    $tts_speak .= "</s>";
}

print "Units: \n" . $tts_speak;
print "\n\n";

my $box = '317';
#print "Box: " . &formatTextString( $box, 'box');
exit;

sub formatTextString
{
    my $string = shift;
    my $category = shift;
    
    $string = &trim( $string ); 
    return unless $string;
    
    print "Applying formatting rules to string [$string] against category [$category]\n";
    
    foreach my $_reg ( @{ $regex_rules->{ $category } } )
    {    
        my $regex_s = qr/$_reg->{search}/;
        if ( $string =~ m/$regex_s/ )
        {
            print "Regex Rules: [$_reg->{search}] [$_reg->{replace} \n";
            my $regex_r = eval $_reg->{replace};
            $string =~ s/$regex_s/$regex_r->()/e;
        }
    }                       

    return $string;
}

sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}
exit;
