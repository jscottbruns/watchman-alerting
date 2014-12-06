#!perl
use DBI;
use SOAP::Lite;
use URI::Escape;

my $subject = "[28] APT GAS";
my $msg = "7521 RIVERDALE RD (FONTAINBLEAU DR & FONTAINBLEAU DR)";
my $lic = "MD160026";
my $inc_no = 'P101250359';
my $q_location;

my @recip;
push( @recip,  "4432501272\@ATT_Wireless_WCTP" );

open(FH, "<P101250359-1");
while (<FH>){
	$data .= $_;
}
close FH;

my $args = {
	'service'   =>  'sms_send',
	'license'   =>  $lic,
	'recip'     =>  [ @recip ],
	'subject'   =>  $subject,
	'message'   =>  $msg,
	'debug'     =>  1,
	'data'		=>	$data,
	'inc_no'	=>	$inc_no,
	'q_location' => $q_location
};

print "Calling SOAP LITE for incident SMS test\n\n";

my $response;
unless ( $response = SOAP::Lite->uri( "http://www.soaplite.com/Watchman/Test", 'timeout' => 5 )->proxy( "http://67.217.167.22:80" )->init( $args )->result ) {

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