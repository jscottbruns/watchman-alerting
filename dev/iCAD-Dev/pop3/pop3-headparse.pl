#!/usr/bin/perl
use DateTime;

my $month_names = {
    'Jan'   => 1,
    'Feb'   => 2,
    'Mar'   => 3,
    'Apr'   => 4,
    'May'   => 5,
    'Jun'   => 6,
    'Jul'   => 7,
    'Aug'   => 8,
    'Sep'   => 9,
    'Oct'   => 10,
    'Nov'   => 11,
    'Dec'   => 12
};

my $txt;
open(FH, "<pop3head");
while ( <FH> )
{
    $txt .= $_;
}
close FH;

print "Before: $txt \n";
$txt =~ /^Date:\s+(.*)$/im and $d = $1;
print "Date: $d\n";

#Mon, 15 Oct 2012 19:50:28 -0400

if ( $d =~ /^([a-zA-Z]{3}),\s+([0-9]{1,2})\s([A-Za-z]{3,4})\s([0-9]{4})\s([0-9]{2}):([0-9]{2}):([0-9]{2})\s([+\-0-9]{5})$/ )
{
    $day = $1;
    print "DOM: $2\n";
    print "Mon: $3\n";
    print "Yea: $4\n";
    print "H: $5\n";
    print "M: $6\n";
    print "S: $7\n";
    print "Off: $8\n";
}
$dt = DateTime->new(
    year       => $4,
    month      => $month_names->{ $3 },
    day        => $2,
    hour       => $5,
    minute     => $6,
    second     => $7,
    time_zone  => $8
); 

print $dt->epoch();
