#!/usr/bin/perl
use Mail::POP3Client;
use File::Temp;

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
    for my $i ( 1 .. $count ) 
    {
        my $inc = $pop->Retrieve( $msg )
        foreach ( $pop->Head( $i ) )
        {   
            print "$_ \n";
        }

        print "Fetching message body\n";
        foreach ( $pop->Body( $i ) )
        {
            print "Message Body => $_\n";
        }
        print "Finished processing message $i\n";
        print "\n";
    }
    #my $msg = $pop->Body( $i );
    #print "MSG $i --- START MESSAGE BODY ---\n";
    #print "$msg \n";
    #print "--- END MESSAGE BODY --- \n";
}

$pop->Close();
