#!perl
use DBI;
use SOAP::Lite;
use URI::Escape;

my $subject = "** PENDING INCIDENT ** [33] HOUSE FIRE";
my $msg = "7215 KENT TOWN DR";
my $q_location = "7215 KENT TOWN DR";
my $lic = "MD160026";

my @recip;
push( @recip,  "4432501272\@ATT_Wireless_WCTP" );

my $args = {
	'service'   =>  'sms_send',
	'license'   =>  $lic,
	'recip'     =>  [ @recip ],
	'subject'   =>  $subject,
	'message'   =>  $msg,
	'debug'     =>  1,
	'q_location' => $q_location
};

print "Calling SOAP LITE for Pending Incident SMS Test\n\n";

my $response;
unless ( $response = SOAP::Lite->uri( "http://www.soaplite.com/Watchman/SOAP", 'timeout' => 5 )->proxy( "http://soap.firehousewatchman.com" )->init( $args )->result ) {

	print "Error: $response";
}

print "Response said: $response\n";
if ( $response =~ /(-?[0-9])\n?(.*)?/m ) {

	my $res = $1;
	my $msg = $2;

	if ( $res == 1 ) {
		print "Connection successful\n";
	} else {
		print "Connection Error: $msg\n";
	}
}

sub trim
{
    my $string = shift;
    if ( $string ) {
        $string =~ s/^\s+//;
        $string =~ s/\s+$//;
        return $string;
    }
}