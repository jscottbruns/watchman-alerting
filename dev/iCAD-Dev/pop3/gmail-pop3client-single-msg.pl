#!/usr/bin/perl
use Mail::POP3Client;
use File::Temp;
use Data::Dumper;

my $msg = $ARGV[0] || 1;


my $pop = new Mail::POP3Client(
    HOST    => 'pop.gmail.com',
    PORT    => 995,
    USESSL  => 1
);
$pop->User('pgmd-incident@fhwm.net');
$pop->Pass('fhwm123321');
$pop->Connect() or die ("Can't connect");

my $count = $pop->Count();
print "Found $count Messages\n";

if ( $count < 0 ) 
{
    print $pop->Message();
}
elsif ( $count == 0 )
{
    print "No Messages\n";
}
else
{
    my $inc = $pop->Head( $msg );
    print Dumper($inc);
}

$pop->Close();
