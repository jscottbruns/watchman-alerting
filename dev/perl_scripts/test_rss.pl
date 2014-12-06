#!perl
use SOAP::Lite;

my $params = {
	'service'	=>	'rss_feed',
	'license'   =>  'MD160026',
	'inc_no'    =>  'P101500254',
	'area'      =>  '33',
	'units'     =>  'E828 A848 A818 A830',
	'location'  =>  '9112 WALLACE RD #BSMT (91ST PL & DEAD END)',
	'callGroup' =>  'typeEms',
	'callType'  =>  'ALS',
	'typeName'  =>  'ALS',
	'text'      =>  '#101500254, 1.She is completely alert (responding appropriately). #10150025'
};

my $response;
unless ( $response = SOAP::Lite->uri( "http://www.soaplite.com/Watchman/SOAP", 'timeout' => 5 )->proxy( "http://soap.firehousewatchman.com" )->init( $params )->result ) {

	print "Error: $response";
}

print "Response said: $response\n";
if ( $response =~ /(-?[0-9])\n?(.*)?/m ) {
	if ( $1 == 1 ) {
        print "Connection successful\n";
	} else {
        print "Connection Error: $msg\n";
	}
}